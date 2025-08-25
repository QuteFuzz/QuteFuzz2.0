#include <qubit_op.h>
#include <block.h>

Qubit_op::Qubit_op(std::shared_ptr<Block> current_block):
    Node("qubit_op", Common::qubit_op)
{
    bool can_use_subroutine = current_block->get_can_apply_subroutines();

    if(!can_use_subroutine){
        add_constraint(Common::subroutine_op, 0);
    }
}

void Qubit_op::add_gate_if_subroutine(std::vector<std::shared_ptr<Node>>& subroutine_gates){
    if(gate_node.has_value() && gate_node.value()->is_subroutine_gate()){
        subroutine_gates.push_back(gate_node.value());
    }
}

std::string Qubit_op::resolved_name() const {
    std::string _string = "UNKNOWN";
    
    if(gate_node.has_value()){
        _string = gate_node.value()->get_string();
    }

    return _string + ", id: " + std::to_string(id);
}