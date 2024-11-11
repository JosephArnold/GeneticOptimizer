template <typename T>
T calculate_fitness(std::vector<uint8_t>& chromosome) {

    T score = 0.0;

    for(uint32_t i = 0; i < chromosome.size(); i++) {

        score += chromosome[i] * 100.0; 

    }

    return score;
} 
