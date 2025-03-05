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

namespace fs = std::filesystem;

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

inline bool string_is(const std::string& string, const std::string& pattern){
    return std::regex_match(string, std::regex(pattern));
}

#endif

