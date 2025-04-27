#include "../include/utils.h"

int Common::Qreg::count = 0;

const std::unordered_map<Common::Rule_hash, std::string> Common::COMMON_TOKEN_STR = {
    {lparen, "("},
    {rparen, ")"},
    {lbrack, "["},
    {rbrack, "]"},
    {lbrace, "{"},
    {rbrace, "}"},
    {comma, ","},
    {space, " "},
    {dot, "."},
    {single_quote, "\'"},
    {double_pipe, "||"},
    {double_quote, "\""},
    {double_ampersand, "&&"},
    {equals, " = "},
    {newline, "\n"}
};

bool Common::is_common(const Rule_hash& hashed_str){
    return COMMON_TOKEN_STR.find(hashed_str) != COMMON_TOKEN_STR.end();
}

bool Common::is_common(const std::string& str){
    Rule_hash hashed_str = (Rule_hash)hash_rule_name(str);
    return is_common(hashed_str);
}

std::string Common::terminal_value(const Rule_hash& hashed_str){
    return COMMON_TOKEN_STR.at(hashed_str);
}

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

/// @brief Create qregs and qubit definitions
/// @param qreg_defs 
void Common::setup_qregs(Qreg_definitions& qreg_defs){
    qreg_defs.reset();

    Qreg::count = 0;

    int num_qubits = random_int(MAX_QUBITS, MIN_QUBITS);
    
    while(num_qubits > 0){
        size_t qreg_size = random_int(MIN_QUBITS, 1);
        qreg_defs.push_back(Qreg(qreg_size));

        num_qubits -= qreg_size;
    }  

    #if 0
    std::cout << qreg_defs;
    #endif
}