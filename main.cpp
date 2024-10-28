// C++ program to create target string, starting from 
// random string using Genetic Algorithm 

#include <bits/stdc++.h>
#include <bitset>
#include <random> 

#define POPULATION_SIZE 50 
#define INDIVIDUAL_SIZE 5

#include "Util.cpp"
#include "Fitness.cpp"

template <typename FitnessDataType>
class Individual { 

    public: 
    std::bitset<INDIVIDUAL_SIZE> chromosome; 
    FitnessDataType fitness; 
    
    Individual(std::bitset<INDIVIDUAL_SIZE> chromosome):chromosome(chromosome) {
        fitness = calculate_fitness<FitnessDataType>(chromosome);
    };
 
    Individual mate(Individual parent2);  

};  

// Perform mating and produce new offspring using Uniform crossover
// Genes are chosen from either parent randomly. 10% of the genes are random.
template <typename FitnessDataType>
Individual<FitnessDataType> Individual<FitnessDataType>::mate(Individual<FitnessDataType> par2) { 
	// chromosome for offspring 
    std::bitset<INDIVIDUAL_SIZE> child_chromosome; 

    for(auto i = 0;i < INDIVIDUAL_SIZE;i++) { 
		// random probability 
        auto p = getRandom(0.0, 100.0) / 100.0; 

	// if prob is less than 0.45, insert gene 
	// from parent 1 
	if(p < 0.45) 
	    child_chromosome[i] = chromosome[i]; 

	// if prob is between 0.45 and 0.90, insert 
	// gene from parent 2 
	else if(p < 0.90) 
	    child_chromosome[i] = par2.chromosome[i]; 

	// otherwise insert random gene(mutate), 
	// for maintaining diversity 
	else
	    child_chromosome[i] = getRandomBinaryDigit(); 
	
    } 

    // create new Individual(offspring) using 
    // generated chromosome for offspring 
    return Individual<FitnessDataType>(child_chromosome); 

};  

// Overloading < operator 
template<typename FitnessDataType>
auto operator<(const Individual<FitnessDataType> &ind1, 
	       const Individual<FitnessDataType> &ind2) -> bool 
{ 
	return ind1.fitness < ind2.fitness; 
} 

// Driver code 
auto main()->int { 

    auto generation = 0; 

    std::vector<Individual<int32_t>> population; 
    bool found = false; 

	// create initial population 
    for(int32_t i = 0;i < POPULATION_SIZE;i++) { 
    
        auto gnome =  create_gnome(); 
	population.push_back(Individual<int32_t>(gnome)); 
	
    } 

    while(!found) { 
	// sort the population in increasing order of fitness score 
        std::sort(population.begin(), population.end()); 

	// if the individual having lowest fitness score ie. 
	// 0 then we know that we have reached to the target 
	// and break the loop 
	if(population[0].fitness <= 0) { 
	    found = true; 
	    break; 
	} 

	// Otherwise generate new offsprings for new generation 
	std::vector<Individual<int32_t>> new_generation; 

	// Perform Elitism, that mean 10% of fittest population 
	// goes to the next generation 
	int32_t s = (10U * POPULATION_SIZE) / 100U; 
	
	for(auto i = 0;i < s;i++) 
	    new_generation.push_back(population[i]); 

	// From 50% of fittest population, Individuals 
	// will mate to produce offspring 
	s = (90U * POPULATION_SIZE) / 100U; //get other 90% of population

	/* For the remaining 90% of the next generation, crossover from the top 50 parents
 	 * chosen at random
 	 */ 
	
	for(auto i = 0;i < s;i++) { 
	
	    auto r = getRandom(0U, 50U); 
	    Individual parent1 = population[r]; 
	    r = getRandom(0U, 50U); 
	    Individual parent2 = population[r]; 
	    Individual offspring = parent1.mate(parent2); 
	    new_generation.push_back(offspring);
	
	}
		
	population = new_generation; 
	std::cout<< "Generation: " << generation << "\t"; 
	std::cout<< "Genome of best individual: "<< population[0].chromosome <<"\t"; 
	std::cout<< "Fitness score of best individual: "<< population[0].fitness << "\n"; 

	generation++; 
	
    }
 
    std::cout<< "Completed" <<std::endl;
    std::cout<< "Generation: " << generation << "\t"; 
    std::cout<< "Solution: "<< population[0].chromosome <<"\t"; 
    std::cout<< "Fitness: "<< population[0].fitness << "\n"; 

} 

