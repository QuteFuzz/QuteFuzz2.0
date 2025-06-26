#include "../include/utils.h"

std::vector<std::vector<int>> Common::QUBIT_COMBINATIONS[MAX_QUBITS-MIN_QUBITS][MAX_QUBITS-MIN_N_QUBITS_IN_ENTANGLEMENT];

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

/// @brief Find all possible combinations that can be chosen from numbers in [0, n-1]
/// Knew the solution had something to do with counting in binary, but I didn't come up with this algorithm myself
/// https://stackoverflow.com/questions/12991758/creating-all-possible-k-combinations-of-n-items-in-c
/// @param n 
/// @param r 
/// @return 
std::vector<std::vector<int>> n_choose_r(const int n, const int r){
    assert((n >= r) && (r >= Common::MIN_N_QUBITS_IN_ENTANGLEMENT));

    std::vector<std::vector<int>> res;

    std::string bitmask(r, 1);
    bitmask.resize(n, 0);

    do{
        std::vector<int> comb;

        for(int i = 0; i < n; i++){
            if (bitmask[i]) comb.push_back(i);
        }

        res.push_back(comb);

    } while(std::prev_permutation(bitmask.begin(), bitmask.end()));

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

void set_possible_qubit_combinations(){

    for(int n_qubits = Common::MIN_QUBITS; n_qubits <= Common::MAX_QUBITS; n_qubits++){

        #if 0
        std::cout << "N qubits: " << n_qubits << std::endl;
        std::cout << "==================================" << std::endl;
        #endif

        for(int n_qubits_in_entanglement = Common::MIN_N_QUBITS_IN_ENTANGLEMENT; n_qubits_in_entanglement <= n_qubits; n_qubits_in_entanglement++){
            
            #if 0
            std::cout << "n_qubits_in_entanglement: " << n_qubits_in_entanglement << std::endl;
            #endif

            Common::QUBIT_COMBINATIONS[n_qubits][n_qubits_in_entanglement] = std::move(n_choose_r(n_qubits, n_qubits_in_entanglement));

            #if 0

            for(const std::vector<int>& entanglement : Common::QUBIT_COMBINATIONS[n_qubits][n_qubits_in_entanglement]){
                for(const int& i : entanglement) std::cout << i << " ";
                std::cout << " ";
            }
            std::cout << std::endl;
            #endif
        }
    }
}
