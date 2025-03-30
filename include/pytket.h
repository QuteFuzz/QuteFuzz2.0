#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

typedef enum {
    CIRCUIT = 115,
    CIRCUIT_OBJ = 75,
    LPAREN = 3,
    
} Rule_names;

class Pytket : public Ast {

    public:
        using Ast::Ast;

        void write(fs::path& path) override;

    private:
        std::ofstream& write_imports(std::ofstream& stream);

        std::ofstream& write(std::ofstream& stream, const Node& node) override;

};

#endif

