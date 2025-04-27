#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

class Pytket : public Ast {

    public:
        using Ast::Ast;

    private:
        std::string imports() override {
            return "from sympy import Symbol \n" \
                "from helpers.pytket_helpers import test_circuit_on_passes \n" \
                "from pathlib import Path \n" \
                "from pytket import Circuit, Qubit, Bit \n" \
                "from pytket.circuit import Op, OpType, MultiplexorBox \n";
        }

        std::string compiler_call() override {
            return NOT_IMPLEMENTED("compiler call");
        }
    
};

#endif

