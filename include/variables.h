#ifndef VARIABLES_H
#define VARIABLES_H

#include "utils.h"

struct Qubit{

    public:
        Qubit(){}

        Qubit(std::string name, std::string _index) : qubit_name(name), index(_index), from_register(true) {}
        
        void reset(){used = false;}

        void set_used(){used = true;}
        
        bool is_used(){return used;}
        
        bool is_from_register(){return from_register;}

        inline std::string get_name(bool with_index = false){return (with_index ? qubit_name + "_" + index : qubit_name);}
        
        inline std::string get_index_as_string(){return index;}

        friend std::ostream& operator<<(std::ostream& stream, Qubit qb){
            stream << qb.qubit_name << "[" << qb.index << "] used: " << qb.used;
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


/*
    An abstraction containing variables used within a circuit block
    - qubit register
    - qubit : internal_qubits are defined within the circuit_block, external_qubits are defined within and exposed as parameters to the circuit block
    - variable
*/
struct Variables {

    public:

        Variables(std::string _circuit) : circuit(_circuit) {}

        inline std::string owner(){
            return circuit;
        }

        /// @brief Number of qubits defined by this block. Counts external qubits by default 
        /// @param external_qubits 
        /// @return 
        inline size_t num_qubits(bool external = true){
            return (external ? external_qubits.size() : internal_qubits.size());
        }

        inline size_t total_num_qubits(){
            return external_qubits.size() + internal_qubits.size();
        }

        inline size_t num_qregs(){
            return qregs.size();
        }

        /// @brief Add qubit register to circuit block. Also decomposes it into its qubits, making them in external by default
        /// @param qreg 
        /// @param external 
        void push_back(const Qreg& qreg, bool external = true){
            qregs.push_back(std::make_shared<Qreg>(qreg));

            if(external) qreg.make_qubits(external_qubits);
            else qreg.make_qubits(internal_qubits);
        }

        /// @brief Add a single qubit to the circuit block. Make it external by default
        /// @param qubit 
        /// @param external 
        void push_back(const Qubit& qubit, bool external = true){
            if(external) external_qubits.push_back(std::make_shared<Qubit>(qubit));
            else internal_qubits.push_back(std::make_shared<Qubit>(qubit));
        }

        /// @brief Reset all stores and pointers
        void reset(){
            qregs.clear();
            external_qubits.clear();
            internal_qubits.clear();
            qreg_pointer = 0;
        }

        /// @brief Reset used flag for each qubit so that it can be chosen for this gate or subroutine
        void qubit_flag_reset(){
            for(auto& qb : external_qubits){
                qb->reset();
            }

            for(auto& qb : internal_qubits){
                qb->reset();
            }
        }
    
        inline std::shared_ptr<Qreg> get_next_qreg(){
            if((size_t)qreg_pointer < qregs.size()){
                return qregs[qreg_pointer++];
            } else {
                return DEFAULT_QREG;
            }
        }

        std::shared_ptr<Qubit> get_random_qubit(std::optional<std::vector<int>> best_entanglement);

        std::shared_ptr<Qubit> get_random_singular_qubit();

        std::shared_ptr<Qubit> get_qubit_at(size_t index);

        friend std::ostream& operator<<(std::ostream& stream, Variables vars){
            stream << "Owner " << vars.circuit << std::endl; 
            stream << "=====================" << std::endl;
            stream << "Qregs, size : " << vars.qregs.size() << std::endl;
            for(const auto& qr : vars.qregs){
                stream << *qr << std::endl;
            }

            stream << "=====================" << std::endl;
            stream << "External Qubits, size: " << vars.external_qubits.size() << std::endl;
            for(const auto& qb : vars.external_qubits){
                stream << *qb << std::endl;
            }

            stream << "=====================" << std::endl;
            stream << "Internal Qubits, size: " << vars.internal_qubits.size() << std::endl;
            for(const auto& qb : vars.internal_qubits){
                stream << *qb << std::endl;
            }

            return stream;
        }
        
        /// @brief Are these qreg definitions owned by this circuit?
        /// @param circuit_name 
        /// @return 
        bool owned_by(std::string circuit_name){
            return circuit == circuit_name;
        }

        size_t setup_qregs(int num_qubits);

        size_t setup_qubits(int num_qubits);

    private:
        std::string circuit;

        /*
            Qregs defined by the user, and qubits, which are derived from the qregs
        */
        std::vector<std::shared_ptr<Qreg>> qregs;
        std::vector<std::shared_ptr<Qubit>> external_qubits;
        std::vector<std::shared_ptr<Qubit>> internal_qubits;
        int qreg_pointer = 0;

};


#endif
