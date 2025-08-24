#include <compound_stmt.h>
#include <block.h>
#include <gate.h>

Compound_stmt::Compound_stmt(int compound_stmt_depth):
    Node("compound_stmt", Common::compound_stmt, indentation_tracker)
{
    if(compound_stmt_depth == 0){
        constraint = std::make_optional<Node_constraint>(Common::qubit_op, 1);
    }
}

std::string Compound_stmt::resolved_name() const {

    std::string _string = "UNKNOWN";
    std::shared_ptr<Node> child = child_at(0), gate_name, gate;
    
    if(child == nullptr){
        ERROR("Compound stmt must have child!");
    }

    if(*child == Common::qubit_op){
        gate_name = find(Common::gate_name);

        if(gate_name == nullptr){
            ERROR("Qubit op must have a gate name under it!");
        }

        gate = gate_name->child_at(0);

        if(gate == nullptr){
            ERROR("Gate name must have child under it!");
        }

        _string = gate->get_string();

    } else {
        _string = string;
    }
    
    return _string + ", id: " + std::to_string(id);
}