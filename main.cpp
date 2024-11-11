// C++ program to create target string, starting from
// random string using Genetic Algorithm

#include <bits/stdc++.h>
#include <bitset>
#include <random>
#include <execution>
#include <GASPI.h>
#include "success_or_die.h"
#define MAX_ITER 100
//#include "Util."
#include "Individual.cpp"
#include "fitness_python.cpp"
#include "CommandLineParser.cpp"

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
    auto POPULATION_SIZE = 1000U;
    auto INDIVIDUAL_SIZE = 100U;

    SUCCESS_OR_DIE( gaspi_proc_init(GASPI_BLOCK) );

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
    
    Individual<float>::individual_size = INDIVIDUAL_SIZE;
    Individual<float>::fitness_function = call_python_function;
    std::vector<Individual<float>> population(POPULATION_SIZE);
    bool found = false;

    // create initial population
//    #pragma omp parallel for
    for (auto i=0U; i < POPULATION_SIZE;i++) {

        //py::gil_scoped_acquire acquire;  // Acquire GIL for each thread
        population[i].setChromosome(create_random_gnome(INDIVIDUAL_SIZE));
  
    }

    std::cout<<"Population initialized in rank"<<rank<<std::endl;

    while (!found) {
        // sort the population in increasing order of fitness score
        std::sort(std::execution::par, population.begin(), population.end());

        // if the individual having lowest fitness score ie.
        // 0 then we know that we have reached to the target
        // and break the loop
        if (generation > MAX_ITER) {
            found = true;
            break;
        }

        // Otherwise generate new offsprings for new generation
        decltype(population) new_generation;

        // Perform Elitism, that mean 10% of fittest population
        // goes to the next generation
        auto percentage = [POPULATION_SIZE](const uint32_t number) -> uint32_t{ 
                                                      return (number * POPULATION_SIZE) / 100U;};
    
        const auto s = percentage(10U);

        std::cout<<"Next generation spawning..."<<std::endl;
    
        std::copy(population.begin(), population.begin() + s, std::back_inserter(new_generation));

        // From the top 50 of the fittest population, Individuals
        // will mate to produce offspring

        for (auto i = s; i < POPULATION_SIZE; i++) {

            auto r = getRandom(0U, 49U);//get random value between 0 and 50
            Individual parent1 = population[r];
            r = getRandom(0U, 49U);
            Individual parent2 = population[r];
            Individual offspring = parent1.mate(parent2);
            new_generation.push_back(offspring);
        }

        population = new_generation;
        std::cout << "Generation: " << generation << "\t";
        //std::cout << "Genome of best individual: " << population[0];
        std::cout << "Fitness score of best individual: " << population[0].fitness << "\n";

        generation++;
  
    }

    std::cout << "Completed" << std::endl;
    std::cout << "Generation: " << generation << "\t";
    std::cout << "Solution: " << population[0];
    std::cout << "Fitness: " << population[0].fitness << "\n";
}
