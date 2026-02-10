#pragma once
#include <random>

// Utility: Generates randomized delay to simulate human input latency.
// This is essential for testing the driver's responsiveness under non-deterministic conditions.
inline int GetRandomDelay(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}