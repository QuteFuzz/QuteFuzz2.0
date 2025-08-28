#ifndef QUBIT_OP_H
#define QUBIT_OP_H

#include <node.h>

class Block;
class Gate;

class Qubit_op : public Node {

    public:
        Qubit_op(): 
            Node("qubit_op", Common::qubit_op)
        {}

        Qubit_op(std::shared_ptr<Block> current_block);

        inline void set_gate_node(std::shared_ptr<Node> node){
            gate_node = std::make_optional<std::shared_ptr<Node>>(node);
        }

        void add_gate_if_subroutine(std::vector<std::shared_ptr<Node>>& subroutine_gates);

        std::string resolved_name() const override;

    private:
        std::optional<std::shared_ptr<Node>> gate_node = std::nullopt;

};

#endif
