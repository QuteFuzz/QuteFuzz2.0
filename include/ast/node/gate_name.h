#ifndef GATE_NAME_H
#define GATE_NAME_H

#include <node.h>

class Gate_name : public Node {

    public:
        using Node::Node;

        Gate_name(const std::shared_ptr<Node> parent):
            Node("gate_name", Common::gate_name)
        {
            if(*parent == Common::subroutine_op){
                constraint = std::make_optional<Node_constraint>(Common::subroutine, 1);
            } else if (*parent == Common::gate_op) {
                constraint = std::make_optional<Node_constraint>(Common::subroutine, 0);
            } else {
                ERROR("Gate name expected parent to be subroutine_op or gate_op!");
            }

        }

    private:

};

#endif