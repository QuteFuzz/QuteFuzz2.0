#include <compound_stmt.h>
#include <block.h>
#include <gate.h>

Compound_stmt::Compound_stmt(unsigned int control_flow_depth):
    Node("compound_stmt", Common::compound_stmt, indentation_tracker)
{
    if(control_flow_depth == 0){
        add_constraint(Common::qubit_op, 1);
    }
}
