#ifndef CONTROL_FLOW_STMT_H
#define CONTROL_FLOW_STMT_H

#include <node.h>

class Control_flow_stmt : public Node {

    public:
        Control_flow_stmt(const std::string& str, U64 hash):
            Node(str, hash)
        {}
    
        Control_flow_stmt(const std::string& str, U64 hash, unsigned int target_num_qubit_ops):
            Node(str, hash)
        {
            /*
                control flow with just compound stmts, or control flow with compound stmts and control flow branch
            */
            unsigned int n_children = (target_num_qubit_ops == 1) || random_int(1) ? 1 : 2;

            make_control_flow_partition(target_num_qubit_ops, n_children);
        }

    private:


};


#endif
