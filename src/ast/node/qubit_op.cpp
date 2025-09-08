#include <qubit_op.h>
#include <block.h>

Qubit_op::Qubit_op(std::shared_ptr<Block> current_block) :
    Node("qubit_op", Common::qubit_op)
{
    /*
        When importing from DAG, dag_copy is passed in and constraints to whether its a gate_op or subroutine_op are set accordingly
    */

    if(!current_block->get_can_apply_subroutines()){
        add_constraint(Common::subroutine_op, 0);
    }
}

std::shared_ptr<Qubit_op> Qubit_op::qubit_op_from_dag(std::shared_ptr<Qubit_op> dag_copy) {
    
    if (dag_copy == nullptr) {
        ERROR("DAG copy of qubit_op is null!");
    }

    bool is_subroutine = false;
    if (dag_copy != nullptr) {
        //Check if the child is a subroutine op or not, which should only have 1 child
        if (dag_copy->get_children().size() == 1) {
            is_subroutine = dag_copy->get_children().at(0)->get_hash() == Common::subroutine_op;
        } else {
            ERROR("DAG copy of qubit_op has unexpected number of children: " + std::to_string(dag_copy->get_children().size()));
        }
    }

    /*
        This will construct a qubit_op node with constraints to match it to DAG
    */

    std::shared_ptr<Qubit_op> new_qubit_op = std::make_shared<Qubit_op>();
    new_qubit_op->add_constraint(is_subroutine ? Common::subroutine_op : Common::gate_op, 1);

    return new_qubit_op;
}

void Qubit_op::add_gate_if_subroutine(std::vector<std::shared_ptr<Node>>& subroutine_gates){
    
    if(gate_node.has_value() && gate_node.value()->is_subroutine_gate()){
        for(std::shared_ptr<Node>& gate : subroutine_gates){
            if(*gate == gate_node.value()->get_string()){return;}
        }

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