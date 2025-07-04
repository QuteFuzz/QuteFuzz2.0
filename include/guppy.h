#ifndef GUPPY_H
#define GUPPY_H

#include "ast.h"

class Guppy : public Ast {

    public:
        using Ast::Ast;

    private:
        std::string imports() override {
            return  "from guppylang.decorator import guppy \n" \
                    "from guppylang.std.angles import angle, pi \n" \
                    "from guppylang.std.builtins import array, owned, py, comptime, result, owned \n" \
                    "from guppylang.std.quantum import * \n" \
                    "from selene_sim import build, Stim \n" \
                    "from diff_testing.lib import guppyTesting \n" \
                    "from pathlib import Path \n";
        }

        std::string compiler_call() override {
            return  "runner = build(guppy.compile(main))\n" \
                    "print(list(runner.run(Stim(), n_qubits=3)))\n" \
                    "main.measure_all() \n" \
                    "gt = guppyTesting() \n" \
                    "gt.run_circ(main, " + std::to_string(build_counter) + ")\n";
        }
    
};

#endif

