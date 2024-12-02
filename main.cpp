// C++ program to create target string, starting from
// random string using Genetic Algorithm

#include <bits/stdc++.h>
#include <bitset>
#include <random>
#include <execution>
#include <GASPI.h>
#include "success_or_die.h"
#define MAX_ITER 100
#include "Individual.cpp"
#include "fitness_python.cpp"
#include "CommandLineParser.cpp"
#include <ctime>
// Overloading < operator
template <typename FitnessDataType>
auto operator<(const Individual<FitnessDataType> &ind1,
               const Individual<FitnessDataType> &ind2) -> bool {
  return ind1.fitness < ind2.fitness;
}

template<typename FitnessDataType>
void operator<<(std::ostream& os, const Individual<FitnessDataType>& ind) {
   
    for (const auto& i:ind.chromosome) {
        os << i<< " ";
    }
    
    os << std::endl;
}


auto main(int argc, char* argv[]) -> int {

    auto generation = 0;
    auto POPULATION_SIZE = 500U;
    auto INDIVIDUAL_SIZE = 100U;

    SUCCESS_OR_DIE( gaspi_proc_init(GASPI_BLOCK) );

    clock_t begin = std::clock();
    gaspi_rank_t rank;
    gaspi_rank_t num;

    SUCCESS_OR_DIE( gaspi_proc_rank(&rank) );
    SUCCESS_OR_DIE( gaspi_proc_num(&num) );

    std::cout<<"Total number of ranks is "<<num<<std::endl;

    InputParser input(argc, argv);
    if(input.cmdOptionExists("-p")){

        const std::string &population_size = input.getCmdOption("-p");
        if (!population_size.empty()){
            POPULATION_SIZE = std::stoi(population_size);
            std::cout<<"Global population size set to "<<POPULATION_SIZE<<std::endl;
        }
        else {
            std::cout<<"Please initialize global population size using -p <value> "<<std::endl;
            return -1;
	}

    }
    else {

         std::cout<<"Default global population size set to "<<POPULATION_SIZE<<std::endl;
    }

    if(input.cmdOptionExists("-i")){

        const std::string &individual_size = input.getCmdOption("-i");
        if (!individual_size.empty()){
            INDIVIDUAL_SIZE = std::stoi(individual_size);
            std::cout<<"Individual size set to "<<INDIVIDUAL_SIZE<<std::endl;
        }
        else {
            std::cout<<"Please initialize individual size using -i <value> "<<std::endl;
            return -1;
        }

    }
     else {

         std::cout<<"Default size of individual genome to "<<INDIVIDUAL_SIZE<<std::endl;
    }

    const auto LOCAL_POPULATION_SIZE = POPULATION_SIZE / num;

    std::cout<<"Creating "<<LOCAL_POPULATION_SIZE<<" individuals in rank "<<rank<<std::endl;

    py::scoped_interpreter guard{};  // Initialize the Python interpreter
    
    Individual<float>::individual_size = INDIVIDUAL_SIZE; //set the size of the individual
    Individual<float>::fitness_function = call_python_function; //assign the fitness function
    std::vector<Individual<float>> population(LOCAL_POPULATION_SIZE);
    bool found = false;

    // create initial population
//    #pragma omp parallel for
    for (auto i=0U; i < LOCAL_POPULATION_SIZE;i++) {

        //py::gil_scoped_acquire acquire;  // Acquire GIL for each thread
        population[i].setChromosome(create_random_gnome(INDIVIDUAL_SIZE));
  
    }

    std::cout<<"Population initialized in rank"<<rank<<std::endl;

    gaspi_segment_id_t const segment_id = 0;

    gaspi_size_t const segment_size = 2 * (sizeof(float) + INDIVIDUAL_SIZE * sizeof (uint32_t));

    /*First half of segment memory will contain local best.
     * Second half will contain incoming best */ 

    SUCCESS_OR_DIE
    ( gaspi_segment_create
      ( segment_id, segment_size
      , GASPI_GROUP_ALL, GASPI_BLOCK, GASPI_MEM_UNINITIALIZED
      )
    );

    gaspi_pointer_t array;
    
    SUCCESS_OR_DIE( gaspi_segment_ptr (segment_id, &array) );

    std::cout<<"segment created in "<<rank<<std::endl;

    auto prev_best_score = std::numeric_limits<float>::max();

    while (!found) {
        // sort the population in increasing order of fitness score
        std::sort(std::execution::par, population.begin(), population.end());

        // Have reached the maximum number of iterations?
        if (generation > MAX_ITER) {
            found = true;
            break;
        }

	/*Each process will have its segment id. It will write the genome of the best individual
	 * to its own segment and the fitness score. For the next generation, it will 
	 * scan the segments of each process and see the what the fitness score of the 
	 * best genome of that process. It will take best out of them and add it to the 
	 * next generation.
	 * We also write out best genome value but we do not notify.
	 * We will stick to writing only the best genome and the fitness score.
	 * Each process will have a segment with id 0.
	 * Memory buffer - [float fitness value, genome]
	 */

	const float curr_best_score = population[0].fitness;

	uint32_t* chromosome_sgt;

	const gaspi_notification_t notify_value = 1; // Notification value

	const gaspi_notification_id_t notify_id_base = 0; // Base ID for notifications

	const gaspi_queue_id_t queue_id = 0;

	if(curr_best_score < prev_best_score) {

	    prev_best_score = curr_best_score;

	    float *src_array = static_cast<float*>(array);

	    *src_array = curr_best_score;

	    std::cout<<"New best score of "<<*src_array<<" written to local segment by rank "<<rank<<std::endl;

	    src_array++; //update src_array to point to data after the fitness score

	    std::vector<uint32_t> best_chromosome = population[0].chromosome;

	    chromosome_sgt = reinterpret_cast<uint32_t*>(src_array);

	    /*copy best chromosome to segment */
	    for(const auto gene:best_chromosome) {

	        *chromosome_sgt = gene;
		//std::cout<<"<<*src_array<<"written to local segment by rank "<<rank<<std::endl;
		chromosome_sgt++;
	    }

	    std::cout<<"Updated information in local segment memory in rank "<<rank<<std::endl;

	    /*Notify all ranks that a new best fitness score has been found in the local population */
	    for (gaspi_rank_t i = 0; i < num; i++) {
                if (i != rank) {
                    SUCCESS_OR_DIE(gaspi_notify(segment_id, i, 
				   notify_id_base + rank, notify_value, queue_id, GASPI_BLOCK));
                }
            }

	}

	float best_incoming_fitness = curr_best_score;

        std::vector<uint32_t> best_incoming_genome = population[0].chromosome;;

	/*Now check for notifications from other processes. Has any process updated its best fitness score? */
        for (gaspi_rank_t i = 0; i < num; i++) {
        
	    if (i != rank) {
            
	        gaspi_notification_id_t received_id;
                gaspi_notification_t received_value;
                gaspi_return_t ret = gaspi_notify_waitsome(segment_id, notify_id_base + i, 1, &received_id, GASPI_TEST);
            
		if (ret == GASPI_SUCCESS) {
                    SUCCESS_OR_DIE(gaspi_notify_reset(segment_id, received_id, &received_value));
	
		    size_t REMOTE_OFF = sizeof(float) + INDIVIDUAL_SIZE * sizeof (uint32_t);

	            /*Check for best scores in others processes local segment */
            
	            SUCCESS_OR_DIE
                    ( gaspi_read(segment_id, REMOTE_OFF, i, 
			      segment_id, 0, REMOTE_OFF, 
			      queue_id, GASPI_BLOCK));

		    SUCCESS_OR_DIE(gaspi_wait(queue_id, GASPI_BLOCK));//wait for read to complete
              
		    float *recv_array = static_cast<float*>(array) + (INDIVIDUAL_SIZE + 1);

		    if(best_incoming_fitness >  *recv_array) {

		        best_incoming_fitness = *recv_array;

		        std::cout<<"Better incoming fitness of "<<best_incoming_fitness
				 <<" received from rank "<<i<<" in rank "<<rank<<std::endl;

		        /*copy chromosome as well */

		        recv_array++; //increase pointer to point to element after fitness value

		        chromosome_sgt = reinterpret_cast<uint32_t*>(recv_array);

		        for(auto& c:best_incoming_genome) {

		            c = *chromosome_sgt;
			
			    chromosome_sgt++;
		        }
   
		    }
	    
	        }

	    }

	}

        // Otherwise generate new offsprings for new generation
        decltype(population) new_generation;

	/*Let the best indiviual from the incoming genome take over */
        new_generation.push_back(Individual<float>(best_incoming_genome));

        auto percentage = [LOCAL_POPULATION_SIZE](const uint32_t number) -> uint32_t{ 
                                                      return (number * LOCAL_POPULATION_SIZE) / 100U;};
    
	// Perform Elitism, that mean 10% of fittest population
        // goes to the next generation

        const auto s = percentage(10U);

        std::cout<<"Next generation spawning in rank "<<rank<<std::endl;
    
        std::copy(population.begin(), population.begin() + s, std::back_inserter(new_generation));

        // From the top 50 of the fittest population, Individuals
        // will mate to produce offspring
	// New generation has s+1 members now

        for (auto i = s+1; i < LOCAL_POPULATION_SIZE; i++) {

            auto r = getRandom(0, 49);//get random value between 0 and 50
            Individual parent1 = population[r];
            r = getRandom(0U, 5U);
            Individual parent2 = new_generation[r];
            Individual offspring = parent1.mate(parent2);
            new_generation.push_back(offspring);
        }

        population = new_generation;
        std::cout << "Generation: " << generation << "\t";
        //std::cout << "Fitness score of best individual: " << population[0].fitness << "\n";

        generation++;
  
    }

    std::cout << "Completed" << std::endl;
    std::cout << "Generation: " << generation << "\t";
    std::cout << "Solution: " << population[0];
    std::cout << "Fitness: " << population[0].fitness << "\n";

    clock_t end = std::clock();
    double elapsed_time = double(end - begin) / CLOCKS_PER_SEC;
    double max_elapsed_time = elapsed_time; // Initialize with local time

    SUCCESS_OR_DIE(gaspi_allreduce(&elapsed_time, &max_elapsed_time, 1, GASPI_OP_MAX, GASPI_TYPE_DOUBLE, GASPI_GROUP_ALL,                   GASPI_BLOCK));

    std::cout<<"Maximum elapsed time among all processes: "<<max_elapsed_time<<std::endl;;
    // Print results on rank 0
    if (rank == 0) {
        std::cout<<"Maximum elapsed time among all processes: "<<max_elapsed_time<<std::endl;;
    }

    SUCCESS_OR_DIE(gaspi_proc_term (GASPI_BLOCK));


}
