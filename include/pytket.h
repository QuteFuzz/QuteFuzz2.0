#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

class Pytket : public Ast {

    public:
        using Ast::Ast;

        void add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints) override;

    private:
        std::string imports(){
            return "from sympy import Symbol \n" \
                "from helpers.pytket_helpers import test_circuit_on_passes \n" \
                "from pathlib import Path \n" \
                "from pytket import Circuit, Qubit, Bit \n" \
                "from pytket.circuit import Op, OpType, MultiplexorBox \n";
        }

        std::string compiler_call(){
            return NOT_IMPLEMENTED("compiler call");
        }
    
        Common::Qreg_definitions qreg_defs;
        Common::Qreg qreg_to_write;

};

#endif

