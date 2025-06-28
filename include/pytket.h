#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

class Pytket : public Ast {

    public:
        using Ast::Ast;

    private:
        std::string imports() override {
            return  "from sympy import Symbol \n" \
                    "from diff_testing.lib import pytketTesting \n" \
                    "from pathlib import Path \n" \
                    "from pytket import Circuit, Qubit, Bit \n" \
                    "from pytket.circuit import Op, OpType, MultiplexorBox, CircBox \n";
        }

        std::string compiler_call() override {
            return  "main_circ.measure_all() \n" \
                    "pt = pytketTesting() \n" \
                    "pt.run_circ(main_circ, " + std::to_string(build_counter) + ")\n";
        }
    
};

#endif

