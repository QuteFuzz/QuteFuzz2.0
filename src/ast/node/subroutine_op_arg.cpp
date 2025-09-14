#include <subroutine_op_arg.h>
#include <resource_definition.h>

/// @brief Dummy argument
Subroutine_op_arg::Subroutine_op_arg() :
    Node("subroutine_op_arg", Common::subroutine_op_arg)
{}

Subroutine_op_arg::Subroutine_op_arg(const std::shared_ptr<Qubit_definition>& qubit_def) :
    Node("subroutine_op_arg", Common::subroutine_op_arg)
{

    if(qubit_def->is_register_def()){
        add_constraint(Common::qubit_list, 1);
    
    } else {
        add_constraint(Common::qubit, 1);
    }
}