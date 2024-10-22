// C++ program to create target string, starting from 
// random string using Genetic Algorithm 

#include <bits/stdc++.h>
#include <bitset>
#include <random> 

#define POPULATION_SIZE 50 
#define INDIVIDUAL_SIZE 5
// Valid Genes 
//const string GENES = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890, .-;:_!\"#%&/()=?@${[]}"; 

#include "Util.cpp"
#include "Fitness.cpp"

// Class representing individual in population 
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

// Perform mating and produce new offspring 
template <typename FitnessDataType>
Individual<FitnessDataType> Individual<FitnessDataType>::mate(Individual<FitnessDataType> par2) 
{ 
	// chromosome for offspring 
	std::bitset<INDIVIDUAL_SIZE> child_chromosome; 

	for(int i = 0;i < INDIVIDUAL_SIZE;i++) 
	{ 
		// random probability 
		auto p = getRandom(0.0, 100.0)/100.0; 

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
bool operator<(const Individual<FitnessDataType> &ind1, 
	       const Individual<FitnessDataType> &ind2) 
{ 
	return ind1.fitness > ind2.fitness; 
} 

// Driver code 
int main() { 

    srand((unsigned)(time(0))); 

    // current generation 
    int generation = 0; 

    std::vector<Individual<int32_t>> population; 
    bool found = false; 

	// create initial population 
    for(int i = 0;i<POPULATION_SIZE;i++) { 
    
        std::bitset<INDIVIDUAL_SIZE> gnome =  create_gnome(); 
	population.push_back(Individual<int32_t>(gnome)); 
	
    } 

    while(!found) { 
		// sort the population in increasing order of fitness score 
		std::sort(population.begin(), population.end()); 

		// if the individual having lowest fitness score ie. 
		// 0 then we know that we have reached to the target 
		// and break the loop 
		if(population[0].fitness >= 500) 
		{ 
			found = true; 
			break; 
		} 

		// Otherwise generate new offsprings for new generation 
		std::vector<Individual<int32_t>> new_generation; 

		// Perform Elitism, that mean 10% of fittest population 
		// goes to the next generation 
		int s = (10*POPULATION_SIZE)/100; 
		for(int i = 0;i<s;i++) 
			new_generation.push_back(population[i]); 

		// From 50% of fittest population, Individuals 
		// will mate to produce offspring 
		s = (90*POPULATION_SIZE)/100; 
		for(int i = 0;i<s;i++) 
		{ 
			int len = population.size(); 
			int r = getRandom(0, 50); 
			Individual parent1 = population[r]; 
			r = getRandom(0, 50); 
			Individual parent2 = population[r]; 
			Individual offspring = parent1.mate(parent2); 
			new_generation.push_back(offspring);
		}
		
		population = new_generation; 
		std::cout<< "Generation: " << generation << "\t"; 
		std::cout<< "Genome: "<< population[0].chromosome <<"\t"; 
		std::cout<< "Fitness: "<< population[0].fitness << "\n"; 

		generation++; 
	}
 
	std::cout<< "Generation: " << generation << "\t"; 
	std::cout<< "String: "<< population[0].chromosome <<"\t"; 
	std::cout<< "Fitness: "<< population[0].fitness << "\n"; 
} 

