#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <queue>
#include <stack>
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
#include <optional>
#include <array>
#include "assert.h"

#ifdef DEBUG 
    #define WILDCARD_MAX 10
#else
    #define WILDCARD_MAX 20
#endif

#define UNUSED(x) (void)(x)
#define NOT_IMPLEMENTED(x) ("# [" x "] NOT IMPLEMENTED! \n")
#define PLACEHOLDER(x) ("<" x ">")
#define ERROR(x) std::cerr << "[ERROR] " << x << std::endl
#define INFO(x) std::cout << "[INFO] " << x << std::endl
#define TOKENS_GRAMMAR_NAME "tokens"

using U64 = uint64_t;

namespace fs = std::filesystem;

U64 hash_rule_name(std::string rule_name);

void lower(std::string& str);

int random_int(int max, int min = 0);

float random_float(float max, float min = 0.0);

int get_amount(int from, int resmin, int resmax);

std::optional<int> safe_stoi(const std::string& str);

std::vector<std::vector<int>> n_choose_r(const int n, const int r);

void set_possible_qubit_combinations();

int vector_sum(std::vector<int> in);

int vector_max(std::vector<int> in);

void pipe_to_command(std::string command, std::string write);

std::string pipe_from_command(std::string command);

namespace Common {
    constexpr char TOP_LEVEL_CIRCUIT_NAME[] = "main";
    constexpr int MIN_N_QUBITS_IN_ENTANGLEMENT = 2;
    constexpr int MIN_QUBITS = 3; 
    constexpr int MAX_QUBITS = std::max(MIN_QUBITS + 1, (int)(0.4 * WILDCARD_MAX));
    constexpr int MAX_SUBROUTINES = (0.2 * WILDCARD_MAX);

    extern bool plot;
    extern bool verbose; 

    struct Qubit_combinations{
        public:
            void set(int num_qubits, int num_qubits_in_entanglement, std::vector<std::vector<int>>& entanglements){
                data[num_qubits-1][num_qubits_in_entanglement-1] = entanglements;
            }

            std::vector<std::vector<int>> at(int num_qubits, int num_qubits_in_entanglement) const {
                return data[num_qubits-1][num_qubits_in_entanglement-1];
            }

            friend std::ostream& operator<<(std::ostream& stream, Qubit_combinations& combs){

                for(int n_qubits = Common::MIN_N_QUBITS_IN_ENTANGLEMENT; n_qubits <= Common::MAX_QUBITS; n_qubits++){

                    stream << "N qubits: " << n_qubits << std::endl;
                    stream << "==================================" << std::endl;

                    for(int n_qubits_in_entanglement = Common::MIN_N_QUBITS_IN_ENTANGLEMENT; n_qubits_in_entanglement <= n_qubits; n_qubits_in_entanglement++){
                        
                        stream << "n_qubits_in_entanglement: " << n_qubits_in_entanglement << std::endl;

                        for(const std::vector<int>& entanglement : combs.at(n_qubits, n_qubits_in_entanglement)){
                            for(const int& i : entanglement) std::cout << i << " ";
                            stream << " ";
                        }
                        stream << std::endl;
                    }
                }

                return stream;
            }

        private:
            std::vector<std::vector<int>> data[MAX_QUBITS][MAX_QUBITS];
    };

    extern Qubit_combinations QUBIT_COMBINATIONS;

    enum Rule_hash : U64 {
        // SINGLE QUBIT GATES
        h = 12638197096160295895ULL,
        x = 12638214688346347271ULL,
        y = 12638213588834719060ULL,
        z = 12638216887369603693ULL,
        rz = 638471042196278621ULL,
        rx = 638468843173022199ULL,
        ry = 638467743661393988ULL,
        u1 = 631765120777144307ULL,
        s = 12638206991764949794ULL,
        sdg = 9383135583349391919ULL,
        t = 12638201494206808739ULL,
        tdg = 6262443406867765286ULL,
        phasedxpowgate = 11832025932794728373ULL,

        // TWO QUBIT GATES
        cx = 622136697450792830ULL,
        ccx = 17716034042247149281ULL,
        u2 = 631766220288772518ULL,
        cz = 622134498427536408ULL,
        cnot = 352634892887392207ULL,
        ch = 622154289636844206ULL,
        crz = 17699985570524998350ULL,
    
        // THREE QUBIT GATES    
        u3 = 631767319800400729ULL,
        cswap = 9091793091986225957ULL,
        toffoli = 15991066036535617246ULL,

        // OTHER GATES
        u = 12638200394695180528ULL,

        // RULE NAMES
        imports = 5522660700275282789ULL,
        block = 1505885265403634530ULL,
        circuit_name = 5389401364268778602ULL,
        float_literal = 6014115549703600901ULL,
        float_literals = 1060812461093276034ULL,
        main_circuit_name = 15359974437464362266ULL,

        body = 14793735007222968981ULL,
        qreg_name = 10722739978486866664ULL,
        qreg_size = 11502232252882731618ULL,
        qubit = 9613145697666387222ULL,
        gate_application = 2267869270411795151ULL,
        gate_application_kind = 6595164713576809234ULL,
        gate_name = 4107851538286704628ULL,
        qubit_list = 18380990572907722739ULL,        
        statement = 7142774524121430294ULL,
        qubit_defs = 10682880043223733083ULL,
        qubit_def = 11301569403047543754ULL,
        qubit_def_singular = 9249986013525299186ULL,
        qubit_def_register = 5722937179312204226ULL,
        singular_qubit_apply = 16057399092026961573ULL,
        register_qubit_apply = 9889855029909000101ULL,

        qubit_name = 8757953683662904688ULL,
        qubit_index = 6830264791288854081ULL,
        subroutines = 3276487754481867520ULL,
        subroutine = 7419198593375467891ULL,
        circuit_id = 12523072865437646660ULL,

        // Guppy rules
        subroutine_parameters = 7194359601375039272ULL,
        subroutine_parameter = 13617813261817958475ULL,
        type = 12075340201627130925ULL,
        parameter_name = 12726161396389809054ULL,
        // assignment = 7458258374037456624ULL,
        // integer_assignment = 15799624338714905563ULL,
        // float_assignment = 15010901291599723643ULL,
        // variable_names = 13035807904021403226ULL,
        // variable_name = 11584133888163274573ULL,
        // function_call = 1406863417753387974ULL,
        angle = 4619207558531745336ULL,
        measure = 13586824709325088415ULL,
        measure_array = 5009991113106631741ULL,
        reset = 1086335023529244512ULL,
        project_z = 2608459580584910443ULL,
        
        // indent = 8881161079555216635ULL,
    };
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

