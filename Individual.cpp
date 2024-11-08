#include "Individual.h"
#include <cassert>

template <typename FitnessDataType>
FitnessDataType (*Individual<FitnessDataType>::fitness_function)(const std::vector<uint32_t>&) = nullptr;

template <typename FitnessDataType>
void Individual<FitnessDataType>::setChromosome(const std::vector<uint32_t> chromosome) {
        assert ((individual_size > 0) && "Initialise fitness function first");
        this->chromosome = chromosome;
        this->fitness = fitness_function(chromosome);
};

template <typename FitnessDataType>
Individual<FitnessDataType>::Individual(const std::vector<uint32_t> chromosome):
                                                         chromosome(chromosome) {
        assert ((individual_size > 0) && "Initialise fitness function first");
        fitness = fitness_function(chromosome);
};

// Perform mating and produce new offspring using Uniform crossover
// Genes are chosen from either parent randomly. 10% of the genes are random.
template <typename FitnessDataType>
Individual<FitnessDataType> Individual<FitnessDataType>::mate(const Individual<FitnessDataType> par2)
     {
    // chromosome for offspring
    std::vector<uint32_t> child_chromosome(individual_size);

    for (auto i = 0; i < individual_size; i++) {
        // random probability
        const auto p = getRandom(0.0, 100.0) / 100.0;

        // if prob is less than 0.45, insert gene
        // from parent 1
        if (p < 0.45)
            child_chromosome[i] = chromosome[i];

        // if prob is between 0.45 and 0.90, insert
        // gene from parent 2
        else if (p < 0.90)
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
