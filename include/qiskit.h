#ifndef QISKIT_H
#define QISKIT_H

#include "ast.h"

class Qiskit : public Ast {

    public:
        using Ast::Ast;

    private:
        std::string imports() override {
            return  "from qiskit import QuantumCircuit, QuantumRegister, ClassicalRegister \n" \
                    "from qiskit.circuit import Parameter, ParameterVector \n" \
                    "from diff_testing.lib import qiskitTesting \n" \
                    "from pathlib import Path \n" \
                    "from math import pi \n";
        }

        std::string compiler_call() override {
            return  "main_circ.measure_active() \n" \
                    "qt = qiskitTesting() \n" \
                    "qt.run_circ(main_circ, " + std::to_string(build_counter) + ")\n";
        }
    
};

#endif