#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

namespace Pytket {
    typedef enum {
        CIRCUIT = 115,
        CIRCUIT_OBJ = 75,
        INT_LITERAL = 71,
        GATE_CALL = 75,
        GATE_NAME = 78,
        H = 73,
        QUBIT_LIST = 45,
        PARAMETER_LIST = 59,
        PARAMETER = 104,
        FLOAT_LITERAL = 69,
        ADD_GATE = 1,
        CX = 24,
        CCX = 88,
        SYMBOL_OBJ = 21,
        IDENTIFIER = 8,
    } Rule_names;

    class Pytket : public Ast {

        public:
            using Ast::Ast;

            void write(fs::path& path) override;

        private:
            std::ofstream& write_imports(std::ofstream& stream);
            std::ofstream& write(std::ofstream& stream, const Node& node) override;

    };
}

#endif

