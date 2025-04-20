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

#define WILDCARD_MAX 50

using U64 = uint64_t;

namespace fs = std::filesystem;

U64 hash_rule_name(std::string rule_name);

void lower(std::string& str);

int random_int(int max, int min = 0);

float random_float(float max, float min = 0.0);

namespace Common {
    constexpr char TOP_LEVEL_CIRCUIT_NAME[] = "main_circ"; 
    constexpr int MIN_QUBITS = 4;
    constexpr int MAX_QUBITS = 15;
    constexpr int MAX_QREGS = 5;

    struct Qreg {
        static int count;

        public:
            Qreg(){}

            Qreg(size_t s){
                name = "qreg" + std::to_string(count++);
                size = s;
            }

            friend std::ostream& operator<<(std::ostream& stream, Qreg q){
                if(q.size == 1){ 
                    stream << q.name << "[0]";

                } else {
                    stream << q.name << "[" << q.size - 1 << ":0]";

                }

                return stream;
            }

            std::string get_name(){return name;}

            size_t get_size(){return size;}

        private:
            std::string name;
            size_t size;
    };

    void setup_qregs(std::vector<Qreg>& qregs);

    enum Common_token : U64 {
        // TOKENS
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

        // SINGLE QUBIT GATES
        h = 12638197096160295895ULL,
        x = 12638214688346347271ULL,
        y = 12638213588834719060ULL,
        z = 12638216887369603693ULL,
        rz = 638471042196278621ULL,
        rx = 638468843173022199ULL,
        ry = 638467743661393988ULL,
        u1 = 631765120777144307ULL,

        // TWO QUBIT GATES
        cx = 622136697450792830ULL,
        ccx = 17716034042247149281ULL,
        u2 = 631766220288772518ULL,
    
        // THREE QUBIT GATES    
        u3 = 631767319800400729ULL,

        // RULE NAMES
        program = 6292337549112776011ULL,
        imports = 5522660700275282789ULL,
        compiler_call = 16764506069072603013ULL,
        circuit = 18088473315674432532ULL,
        circuit_name = 5389401364268778602ULL,
        circuit_def = 17654104105659481736ULL,
        int_literal = 12222978820271297122ULL,
        float_literal = 6014115549703600901ULL,
        gate_name = 4107851538286704628ULL,
        qubit_list = 18380990572907722739ULL,
        parameter_list = 10044088521670889753ULL,
        parameter = 1363275014107747824ULL,
        statements = 7457579184642066079ULL,
        qreg_def = 11422033681961328082ULL,
        qreg_name = 10722739978486866664ULL,
        qreg_size = 11502232252882731618ULL,
        qreg_defs = 3680647047563729043ULL,
        qubit = 9613145697666387222ULL,
        gate_application = 2267869270411795151ULL,
        gate_application_kind = 6595164713576809234ULL,
        statement = 7142774524121430294ULL,
    };

    extern const std::unordered_map<Common_token, std::string> COMMON_TOKEN_STR;

    bool is_common(const Common_token& hashed_str);

    bool is_common(const std::string& str);

    std::string terminal_value(const Common_token& hashed_str);
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

    return multiplied_vec;
}

template<typename T>
std::vector<T> append_vectors(std::vector<T> vec1, std::vector<T> vec2){
    std::vector<T> result = vec1;

    result.insert(result.end(), vec2.begin(), vec2.end());

    return result;
}

#endif

