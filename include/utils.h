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

namespace fs = std::filesystem;

uint64_t hash_rule_name(std::string rule_name);

namespace Common {
    enum Common_token : uint64_t {
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
        cx = 622136697450792830ULL,
        ccx = 17716034042247149281ULL,
    };

    extern const std::unordered_map<Common_token, std::string> COMMON_TOKEN_STR;

    bool is_common(const Common_token& hashed_str);

    bool is_common(const std::string& str);

    std::string terminal_value(const std::string& str);
}

typedef enum{
    R_OK,
    R_ERROR
} Result_kind;

template<typename A, typename B>
struct Result{

    public:
        Result(){}

        ~Result(){}

        void set_ok(A a){
            as = a;
            kind = R_OK;
        }

        void set_error(B b){
            as = b;
            kind = R_ERROR;
        }

        A get_ok() const {
            if(kind == R_OK){return std::get<A>(as);}
            else {
                throw std::runtime_error("get_ok called on error!");
            }
        }

        B get_error() const {
            if(kind == R_ERROR){return std::get<B>(as);}
            else {
                throw std::runtime_error("get_ok called on OK!");
            }
        }

        bool is_ok() const {
            return is(R_OK);
        }

        bool is_error() const {
            return is(R_ERROR);
        }

    private:
        bool is(Result_kind rk) const {
            return kind == rk;
        }

        Result_kind kind;
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

