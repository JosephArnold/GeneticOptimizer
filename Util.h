#include <bitset>
#include <random>


template <typename T>
concept Number = std::floating_point<T> or std::integral<T>;
auto getRandom(Number auto low, Number auto high) {
  // Define a random number generator
  std::random_device rd; // Seed the generator (non-deterministic if available)
  std::mt19937 gen(
      rd()); // Mersenne Twister engine for generating random numbers

  // Define a distribution range for the random integers
  std::uniform_int_distribution<> distr(
      low, high); // Generates numbers between 1 and 100

  return (distr(gen));
}

