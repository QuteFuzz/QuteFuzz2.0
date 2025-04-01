#include "../include/utils.h"

const std::unordered_map<Common::Common_token, std::string> Common::COMMON_TOKEN_STR = {
    {lparen, "("},
    {rparen, ")"},
    {comma, ","},
    {space, " "},
    {dot, "."},
    {single_quote, "\'"},
    {double_pipe, "||"},
    {double_quote, "\""},
    {double_ampersand, "&&"},
    {equals, " = "},
};

bool Common::is_common(const Common_token& hashed_str){
    return COMMON_TOKEN_STR.find(hashed_str) != COMMON_TOKEN_STR.end();
}

bool Common::is_common(const std::string& str){
    Common_token hashed_str = (Common_token)hash_rule_name(str);
    return is_common(hashed_str);
}

std::string Common::terminal_value(const std::string& str){
    Common_token hashed_str = (Common_token)hash_rule_name(str); 

    if(is_common(hashed_str)){
        return COMMON_TOKEN_STR.at(hashed_str);
    } else {
        return str;
    }
}

void lower(std::string& str){
    std::transform(str.begin(), str.end(), str.begin(),
        [](char c){return std::tolower(c);}
    );
}

/// @brief Lowercase input for consistency, and ease. Uses FNV hash. Tried to find a good hash so as to avoid collisions
/// @param rule_name 
/// @return 
uint64_t hash_rule_name(std::string rule_name) {
    uint64_t hash = 14695981039346656037ULL;  // FNV offset basis
    const uint64_t prime = 1099511628211ULL;  // FNV prime

    lower(rule_name);

    for (const char& c : rule_name) {
        hash ^= static_cast<uint8_t>(c);  
        hash *= prime;                     // multiply by a prime
    }

    return hash;
}