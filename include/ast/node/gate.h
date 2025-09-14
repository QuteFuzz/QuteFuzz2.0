#ifndef GATE_H
#define GATE_H

#include <node.h>
#include <collection.h>

class Qubit_definition;

class Gate : public Node {

    public:

        Gate() : 
            Node("dummy")
        {}
        
        /// @brief Use for predefined gates
        /// @param str 
        /// @param _hash 
        /// @param _qubits 
        /// @param _bits 
        /// @param _floats 
        Gate(const std::string& str, U64 _hash, unsigned int _qubits, unsigned int _bits, unsigned int _floats);
        
        /// @brief Use for subroutines
        /// @param str 
        /// @param _hash 
        /// @param qubit_defs 
        Gate(const std::string& str, U64 _hash, const Collection<Qubit_definition>& qubit_defs);

        std::string get_id_as_str(){
            return std::to_string(id);
        }

        unsigned int get_n_ports() const override {return num_external_qubits;}

        unsigned int get_num_external_qubits();
        
        unsigned int get_num_external_qubit_defs() const { return external_qubit_defs.size(); } 

        unsigned int get_num_external_bits() const { return num_external_bits;}

        unsigned int get_num_floats() const {return num_floats;}

        std::shared_ptr<Qubit_definition> get_next_qubit_def();

    private:
        Collection<Qubit_definition> external_qubit_defs;
        unsigned int qubit_def_pointer = 0;

        unsigned int num_external_qubits = 0;
        unsigned int num_external_bits = 0;
        unsigned int num_floats = 0;
};


#endif
