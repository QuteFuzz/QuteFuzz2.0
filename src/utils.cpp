#include "../include/utils.h"

void lower(std::string& str){
    std::transform(str.begin(), str.end(), str.begin(),
        [](char c){return std::tolower(c);}
    );
}

/// @brief Lowercase input for consistency, and ease. Uses FNV hash. Tried to find a good hash so as to avoid collisions
/// @param rule_name 
/// @return 
U64 hash_rule_name(std::string rule_name) {
    U64 hash = 14695981039346656037ULL;  // FNV offset basis
    const U64 prime = 1099511628211ULL;  // FNV prime

    lower(rule_name);

    for (const char& c : rule_name) {
        hash ^= static_cast<uint8_t>(c);  
        hash *= prime;                     // multiply by a prime
    }

    return hash;
}

/// @brief Random integer within some range
/// @param max value inclusive
/// @param min value inclusive
/// @return 
int random_int(int max, int min){
    std::random_device random_device;
    std::mt19937 random_gen(random_device());

    if(min < max){
        std::uniform_int_distribution<int> int_dist(min, max);
        return int_dist(random_gen);

    } else {
        return min;
    }
}

/// @brief Random float within some range
/// @param max value inclusive
/// @param min value inclusive
/// @return 
float random_float(float max, float min){
    std::random_device random_device;
    std::mt19937 random_gen(random_device());

    if(min < max){
        std::uniform_real_distribution<float> float_dist(min, max);
        return float_dist(random_gen);

    } else {
        return min;
    }
}

int get_amount(int from, int resmin, int resmax){
    float frac = (float)from / (float)WILDCARD_MAX;
    int res = resmin + (frac * (resmax - resmin));
    return std::min(res, resmax);
}


std::optional<int> safe_stoi(const std::string& str) {
    try {
        int ret = (str == "") ? 1 : std::stoi(str);
        return std::optional<int>(ret);
    } catch (const std::invalid_argument& e) {
        return std::nullopt;
    }
}

/// @brief Return all possible pairs that can be chosen from numbers in range [0,n-1]
/// @param n 
/// @return 
std::vector<std::pair<int, int>> n_choose_2(int n){
    std::vector<std::pair<int, int>> res;

    for(int i = 0; i < n; i++){
        for(int j = i+1; j < n; j++){
            res.push_back(std::make_pair(i, j));
        }
    }

    return res;
}

int vector_sum(std::vector<int> in){
    int res = 0;

    for(size_t i = 0; i < in.size(); i++){
        res += in[i];
    }

    return res;
}

int vector_max(std::vector<int> in){
    int max = 0;

    for(size_t i = 0; i < in.size(); i++){
        max = std::max(max, in[i]);
    }

    return max;
}