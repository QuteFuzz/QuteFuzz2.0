#include <gate.h>
#include <resource_definition.h>
#include "assert.h"

/// @brief Use for predefined gates
/// @param str 
/// @param _hash 
/// @param _qubits 
/// @param _bits 
/// @param _floats 
Gate::Gate(const std::string& str, U64 _hash, unsigned int _qubits, unsigned int _bits, unsigned int _floats) :
    Node(str),
    num_external_qubits(_qubits),
    num_external_bits(_bits),
    num_floats(_floats)
{
    hash = _hash;
}

/// @brief Use for subroutines. Filters out qubits defs in `EXTERNAL_SCOPE`
/// @param str 
/// @param _hash 
/// @param qubit_defs 
Gate::Gate(const std::string& str, U64 _hash, const Collection<Qubit_definition>& qubit_defs) :
    Node(str)
{
    hash = _hash;

    assert(hash == Common::subroutine);

    // filter out external qubit defs
    for(const auto& qubit_def : qubit_defs){
        if(qubit_def->get_scope() & EXTERNAL_SCOPE){
            external_qubit_defs.add(*qubit_def);
            num_external_qubits += qubit_def->get_size()->get_num();
        }
    }
}

std::shared_ptr<Qubit_definition> Gate::get_next_qubit_def(){
    return external_qubit_defs.at(qubit_def_pointer++ % external_qubit_defs.size());
}

unsigned int Gate::get_num_external_qubits(){
    if(hash == Common::subroutine){
        return get_next_qubit_def()->get_size()->get_num();
    } else {
        return num_external_qubits;
    }
}