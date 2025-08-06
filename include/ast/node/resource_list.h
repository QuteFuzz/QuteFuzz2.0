#ifndef RESOURCE_LIST_H
#define RESOURCE_LIST_H

#include <node.h>

class Resource_list : public Node {
    public:
        Resource_list(std::string str, U64 hash, int num_resources_in_list, bool is_qubit):
            Node(str, hash)
        {
            constraint = std::make_optional<Node_constraint>(is_qubit ? Common::qubit : Common::bit, num_resources_in_list);
        }

    private:
};

#endif
