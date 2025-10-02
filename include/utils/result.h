#ifndef RESULT_H
#define RESULT_H

#include <utils.h>

/*
    inspired by F# ;)
*/

template<typename T>
struct Result{

    public:
        Result(){}

        ~Result(){}

        void set_ok(T val){
            as = val;
        }

        void set_error(const std::string& err){
            as = err;
        }

        T get_ok() const {
            if(is_ok()){return std::get<T>(as);}
            else {
                throw std::runtime_error(ANNOT("get_ok called on error!"));
            }
        }

        std::string get_error() const {
            if(is_error()){return std::get<std::string>(as);}
            else {
                throw std::runtime_error(ANNOT("get_error called on OK!"));
            }
        }

        bool is_ok() const {
            return std::holds_alternative<T>(as);
        }

        bool is_error() const {
            return std::holds_alternative<std::string>(as);
        }

    private:
        std::variant<T, std::string> as;
};


#endif
