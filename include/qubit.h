#ifndef QUBIT_H
#define QUBIT_H

#include "utils.h"

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
            stream << qb.qubit_name << "[" << qb.index << "] used: " << qb.used;
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


struct Qreg_definitions {

    public:

        Qreg_definitions(std::string _circuit) : circuit(_circuit) {}

        std::string owner(){
            return circuit;
        }

        size_t num_qubits(){
            return qubits.size();
        }

        void push_back(Qreg qreg){
            qregs.push_back(qreg);
            qreg.make_qubits(qubits);
        }

        void reset(){
            qregs.clear();
            qubits.clear();
            qreg_pointer = 0;
        }

        /// @brief Reset used flag for each qubit so that it can be chosen for this gate or subroutine
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

        std::shared_ptr<Qubit> get_random_qubit(std::optional<std::vector<int>> best_entanglement);

        friend std::ostream& operator<<(std::ostream& stream, Qreg_definitions defs){
            stream << "Owner " << defs.circuit << std::endl; 
            stream << "=====================" << std::endl;
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
        
        /// @brief Are these qreg definitions owned by this circuit?
        /// @param circuit_name 
        /// @return 
        bool owned_by(std::string circuit_name){
            return circuit == circuit_name;
        }

        size_t setup_qregs(int minimum_num_qubits);

    private:
        std::string circuit;
        std::vector<Qreg> qregs;
        std::vector<Qubit> qubits;
        int qreg_pointer = 0;

};


#endif
