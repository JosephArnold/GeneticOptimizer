template <typename T>
T calculate_fitness(std::bitset<INDIVIDUAL_SIZE>& chromosome) {

    T score = 0.0;

    for(uint32_t i = 0; i < INDIVIDUAL_SIZE; i++) {

        score += chromosome[i] * 100.0; 

    }

    return score;
} 
