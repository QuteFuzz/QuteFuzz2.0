#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

class Pytket : public Ast {

    public:
        using Ast::Ast;

    private:
        std::ofstream& write(std::ofstream& stream, const Node& node) override;

};

#endif

