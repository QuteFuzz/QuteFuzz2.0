#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <variant>
#include <regex>
#include <random>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cstdint>

#define WILDCARD_MAX 2

using U64 = uint64_t;

namespace fs = std::filesystem;

U64 hash_rule_name(std::string rule_name);

namespace Common {
    enum Common_token : U64 {
        lparen = 8662532954183415845ULL,
        rparen = 4240811817421387563ULL,
        comma = 7874411517935695704ULL,
        space = 4858524775536387045ULL,
        dot = 14604936204231399584ULL,
        single_quote = 11907393312445673822ULL,
        double_pipe = 6606020031733204521ULL,
        double_quote = 8080484409244937479ULL,
        double_ampersand = 5535872232892287956ULL,
        equals = 3453683047558497236ULL,

        h = 12638197096160295895ULL,
        x = 12638214688346347271ULL,
        y = 12638213588834719060ULL,
        z = 12638216887369603693ULL,

        cx = 622136697450792830ULL,
        ccx = 17716034042247149281ULL,
        
        rz = 638471042196278621ULL,
        rx = 638468843173022199ULL,
        ry = 638467743661393988ULL,
        
        u1 = 631765120777144307ULL,
        u2 = 631766220288772518ULL,
        u3 = 631767319800400729ULL,
    };

    extern const std::unordered_map<Common_token, std::string> COMMON_TOKEN_STR;

    bool is_common(const Common_token& hashed_str);

    bool is_common(const std::string& str);

    std::string terminal_value(const std::string& str);
}

template<typename A, typename B>
struct Result{

    public:
        Result(){}

        ~Result(){}

        void set_ok(A a){
            as = a;
        }

        void set_error(B b){
            as = b;
        }

        A get_ok() const {
            if(is_ok()){return std::get<A>(as);}
            else {
                throw std::runtime_error("get_ok called on error!");
            }
        }

        B get_error() const {
            if(is_error()){return std::get<B>(as);}
            else {
                throw std::runtime_error("get_ok called on OK!");
            }
        }

        bool is_ok() const {
            return as.index() == 0;
        }

        bool is_error() const {
            return as.index() == 1;
        }

    private:
        std::variant<A, B> as;
};

template<typename T>
std::vector<T> multiply_vector(std::vector<T> vec, int mult){
    std::vector<T> multiplied_vec;
    
    multiplied_vec.reserve(vec.size() * mult);

    for(int i = 0; i < mult; ++i){
        multiplied_vec.insert(multiplied_vec.end(), vec.begin(), vec.end());
    }

    std::cout << multiplied_vec.size() << std::endl;

    return multiplied_vec;
}

template<typename T>
std::vector<T> append_vectors(std::vector<T> vec1, std::vector<T> vec2){
    std::vector<T> result = vec1;

    result.insert(result.end(), vec2.begin(), vec2.end());

    return result;
}

void lower(std::string& str);

#endif

