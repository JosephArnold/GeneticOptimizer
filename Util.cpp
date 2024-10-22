#include <bitset>
#include <random>

uint32_t getRandomBinaryDigit() { 
 // Define a random number generator
    std::random_device rd; // Seed the generator (non-deterministic if available)
    std::mt19937 gen(rd()); // Mersenne Twister engine for generating random numbers

    // Define a distribution range for the random integers
    std::uniform_int_distribution<> distr(1, 100); // Generates numbers between 1 and 100

    // Generate a random number
    return (distr(gen) % 2);

} 

template<typename T>
T getRandom(T low, T high) {
 // Define a random number generator
    std::random_device rd; // Seed the generator (non-deterministic if available)
    std::mt19937 gen(rd()); // Mersenne Twister engine for generating random numbers

    // Define a distribution range for the random integers
    std::uniform_int_distribution<> distr(low, high); // Generates numbers between 1 and 100

    // Generate a random number
    return (distr(gen));

}

// create chromosome or string of genes
std::bitset<INDIVIDUAL_SIZE> create_gnome() {

    std::bitset<INDIVIDUAL_SIZE> genome;

    for(auto i = 0;i<INDIVIDUAL_SIZE;i++) {
        if(getRandomBinaryDigit()) {

            genome.set(i);

        }

    }

    return genome;
}

