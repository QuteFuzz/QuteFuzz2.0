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
#define UNUSED(x) (void)(x)
#define NOT_IMPLEMENTED(x) "# [" x "] NOT IMPLEMENTED! \n"
#define PLACEHOLDER(x) "[PLACEHOLDER] " x
#define ERROR(x) std::cout << "[ERROR] " << x << std::endl
#define TOKENS_GRAMMAR_NAME "tokens"

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

    struct Qubit{
        public:
            Qubit(){}
            Qubit(std::string name, std::string _index) : qubit_name(name), index(_index) {}
            void reset(){used = false;}
            void set_used(){used = true;}
            bool is_used(){return used;}
            inline std::string get_name(){return qubit_name;}
            inline std::string get_index_as_string(){return index;}

            friend std::ostream& operator<<(std::ostream& stream, Qubit qb){
                stream << qb.qubit_name << "[" << qb.index << "]";
                return stream;
            }

        private:
            std::string qubit_name = PLACEHOLDER("qubit_name");
            std::string index = PLACEHOLDER("qubit_index");
            bool used = false;
    };

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
                    stream << q.name << "[0: " << q.size - 1 << "]";

                }

                return stream;
            }

            std::string get_name(){return name;}

            std::string get_size_as_string(){return std::to_string(size);}

            /// @brief Add qubits to the given vector from the qreg
            /// @param qubits 
            void make_qubits(std::vector<Qubit>& qubits){
                for(size_t i = 0; i < size; i++){
                    qubits.push_back(Qubit(name, std::to_string(i)));
                }
            }

        private:
            std::string name = PLACEHOLDER("qubit_name");
            size_t size = 0;
    };

    const std::shared_ptr<Qubit> DEFAULT_QUBIT = std::make_shared<Qubit>(Qubit());
    const std::shared_ptr<Qreg> DEFAULT_QREG = std::make_shared<Qreg>(Qreg());

    struct Qreg_definitions{

        public:

            Qreg_definitions(){}

            void push_back(Qreg qreg){
                qregs.push_back(qreg);
                qreg.make_qubits(qubits);
            }

            void reset(){
                qregs.clear();
                qubits.clear();
                qreg_pointer = 0;
            }

            void reset_qubits(){
                for(Qubit& qb : qubits){
                    qb.reset();
                }
            }
        
            inline std::shared_ptr<Qreg> get_next_qreg(){
                if((size_t)qreg_pointer < qregs.size()){
                    return std::make_shared<Qreg>(qregs[qreg_pointer++]);
                } else {
                    return DEFAULT_QREG;
                }
            }

            inline std::shared_ptr<Qubit> get_random_qubit(){
                if(qubits.size()){
                    int index = random_int(qubits.size() - 1);
                    Qubit* qubit = &qubits[index];

                    while(qubit->is_used()){
                        index = random_int(qubits.size() - 1);
                        qubit = &qubits[index];
                    }

                    qubit->set_used();
                
                    return std::make_shared<Qubit>(qubits[index]);
                
                } else {
                    return DEFAULT_QUBIT;
                }
            }

            friend std::ostream& operator<<(std::ostream& stream, Qreg_definitions defs){
                stream << "Qregs " << std::endl;
                for(const Qreg& qr : defs.qregs){
                    stream << qr << std::endl;
                }

                stream << "=====================" << std::endl;
                stream << "Qubits " << std::endl;
                for(const Qubit& qb : defs.qubits){
                    stream << qb << std::endl;
                }

                stream << "n_qubits: " << defs.qubits.size() << " n_qregs: " << defs.qregs.size() << std::endl;

                return stream;
            }

            inline size_t num_qregs(){
                return qregs.size();
            }

        private:
            std::vector<Qreg> qregs;
            std::vector<Qubit> qubits;
            int qreg_pointer = 0;

    };

    void setup_qregs(Qreg_definitions& qreg_defs);

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
        qubit_name = 8757953683662904688ULL,
        qubit_index = 6830264791288854081ULL,
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

