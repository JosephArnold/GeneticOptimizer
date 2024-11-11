#pragma once 

#include <bitset>
#include <random>
#include "Util.h"


auto getRandomBinaryDigit() {
  // Define a random number generator
  std::random_device rd; // Seed the generator (non-deterministic if available)
  std::mt19937 gen(
      rd()); // Mersenne Twister engine for generating random numbers

  // Define a distribution range for the random integers
  std::uniform_int_distribution<> distr(
      1, 100); // Generates numbers between 1 and 100

  return (distr(gen) % 2);
}

// create chromosome or string of genes
std::vector<uint32_t> create_random_gnome(const uint32_t& individual_size=100) {

    std::vector<uint32_t> genome(individual_size);

    for (auto i = 0; i < individual_size; i++) {

      genome[i] = getRandomBinaryDigit();
  
    }

    return genome;

}

