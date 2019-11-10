#include "UUID.h"
#include <random>
#include <algorithm>

std::string UUID::random() {
    std::uniform_int_distribution<int> dist(0, 15);
    // create random UUID according to this format
    std::string id{"00000000-0000-0000-0000-000000000000"};
    std::for_each(std::begin(id), std::end(id), [&dist](char &digit){
        static std::random_device dev;
        static std::mt19937 rng{dev()};
        static constexpr char digits[]{"0123456789abcdef"};
        if (digit != '-') digit = digits[dist(rng)]; 
    });
    return id;
}
