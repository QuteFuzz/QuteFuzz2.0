#ifndef QUBIT_DEF_H
#define QUBIT_DEF_H

#include <utils.h>

namespace Qubit_def {

    struct Qubit{

        static int count;

        public:
            Qubit(){}

            Qubit(std::string prefix) : qubit_name(prefix + std::to_string(count)) {}

            Qubit(std::string name, std::string _index) : qubit_name(name), index(_index), from_register(true) {}
            
            void reset(){used = false;}

            void set_used(){used = true;}
            
            bool is_used(){return used;}
            
            bool is_from_register(){return from_register;}

            inline std::string get_name(bool with_index = false){return (with_index ? qubit_name + "_" + index : qubit_name);}
            
            inline std::string get_index_as_string(){return index;}

            friend std::ostream& operator<<(std::ostream& stream, Qubit qb){
                if(qb.from_register){
                    stream << qb.qubit_name << "[" << qb.index << "] used: " << qb.used;

                } else {
                    stream << qb.qubit_name << " used: " << qb.used;
                }

                return stream;
            }

        private:
            std::string qubit_name = PLACEHOLDER("qubit_name");
            std::string index = PLACEHOLDER("qubit_index");
            bool from_register = false;

            bool used = false;
    };

    struct Qreg {
        static int count;

        public:
            Qreg(){}

            Qreg(size_t s, std::string prefix = "qreg"){
                name = prefix + std::to_string(count++);
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
            void make_qubits(std::vector<std::shared_ptr<Qubit>>& qubits) const {
                for(size_t i = 0; i < size; i++){
                    qubits.push_back(std::make_shared<Qubit>(name, std::to_string(i)));
                }
            }

        private:
            std::string name = PLACEHOLDER("qubit_name");
            size_t size = 0;
    };

    const std::shared_ptr<Qubit> DEFAULT_QUBIT = std::make_shared<Qubit>(Qubit());
    const std::shared_ptr<Qreg> DEFAULT_QREG = std::make_shared<Qreg>(Qreg());

    enum Type {
        REGISTER,
        SINGULAR,
    };

    struct Qubit_def {

        public:

            Qubit_def(){}

            Qubit_def(std::shared_ptr<Qreg> register_def){
                value = register_def;
                type = REGISTER;
            }

            Qubit_def(std::shared_ptr<Qubit> singular_def){
                value = singular_def;
                type = SINGULAR;
            }

            std::shared_ptr<Qreg> get_register_def() const {
                return std::get<std::shared_ptr<Qreg>>(value);
            }

            std::shared_ptr<Qubit> get_singular_def() const {
                return std::get<std::shared_ptr<Qubit>>(value);
            }

            Type get_type(){
                return type;
            }

        private:

            friend std::ostream& operator<<(std::ostream& stream, Qubit_def qubit_def){

                std::visit([&stream](auto&& value){
                    stream << *value;
                }, qubit_def.value);

                return stream;
            }

            Type type;
            std::variant<std::shared_ptr<Qreg>, std::shared_ptr<Qubit>> value;
    };
}



#endif

