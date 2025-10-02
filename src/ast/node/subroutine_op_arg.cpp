#include <subroutine_op_arg.h>
#include <resource_definition.h>

/// @brief Dummy argument
Subroutine_op_arg::Subroutine_op_arg() :
    Node("subroutine_op_arg", Token::SUBROUTINE_OP_ARG)
{}

Subroutine_op_arg::Subroutine_op_arg(const std::shared_ptr<Qubit_definition>& qubit_def) :
    Node("subroutine_op_arg", Token::SUBROUTINE_OP_ARG)
{

    if(qubit_def->is_register_def()){
        add_constraint(Token::QUBIT_LIST, 1);
    
    } else {
        add_constraint(Token::QUBIT, 1);
    }
}