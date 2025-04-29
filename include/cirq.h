#ifndef CIRQ_H
#define CIRQ_H

#include "ast.h"

class Cirq : public Ast {

    public:
        using Ast::Ast;

    private:
        std::string imports() override {
            // TODO: Add logic to modify imports based on the gates used in the circuit
            return  "from cirq import (Circuit, X, Y, Z, S, T, rx, rz, ry, CNOT, CSWAP, CZ, PhasedXPowGate, \n"\
                    "NamedQubit, InsertStrategy, CircuitOperation, measure)\n"\
                    "from pathlib import Path\n"\
                    "from helpers.cirq_helpers import compare_circuits_after_gateset_optimisation, individual_pass\n";
        }

        std::string compiler_call() override {
            return NOT_IMPLEMENTED("compiler call");
        }
    
};

#endif

