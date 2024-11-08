#include <vector>
#include <cstdint>
#include "Util.cpp"

template <typename FitnessDataType> 
class Individual {

    public:
    std::vector<uint32_t> chromosome;
    FitnessDataType fitness;
    static inline int32_t individual_size=0;
    static FitnessDataType (*fitness_function)(const std::vector<uint32_t>&); 

    static inline void initialize(const int32_t individual_size,
                                  FitnessDataType (*compute_fitness)(const std::vector<uint32_t>&)) {
        individual_size = individual_size;
        fitness_function = compute_fitness;

    };

    Individual()=default;

    Individual(const std::vector<uint32_t>);    
    void setChromosome(const std::vector<uint32_t>);

    Individual mate(const Individual);

    
};


