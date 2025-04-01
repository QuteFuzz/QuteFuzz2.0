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

/// @brief Lowercase input for consistency, and ease
/// @param rule_name 
/// @return 
unsigned int hash_rule_name(std::string rule_name){
    int hash = 0;
    int len = 1;

    lower(rule_name);

    for(const char& c : rule_name){
        hash ^= c ^ (len++);
    }

    return hash;
}