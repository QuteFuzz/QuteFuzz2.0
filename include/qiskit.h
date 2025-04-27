#ifndef QISKIT_H
#define QISKIT_H

#include "ast.h"

class Qiskit : public Ast {

    public:
        using Ast::Ast;

    private:
        std::string imports() override {
            return "from qiskit import QuantumCircuit, QuantumRegister, ClassicalRegister \n" \
                    "from qiskit.circuit import Parameter, ParameterVector \n" \
                    "from helpers.qiskit_helpers import compare_statevectors, run_on_simulator, run_routing_simulation, run_pass_on_simulator \n" \
                    "from pathlib import Path \n" \
                    "from math import pi \n";
        }

        std::string compiler_call() override {
            return NOT_IMPLEMENTED("compiler call");
        }
    
};

#endif