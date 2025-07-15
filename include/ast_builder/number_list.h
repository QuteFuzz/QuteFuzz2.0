#ifndef FLOAT_LIST_H
#define FLOAT_LIST_H

#include <node.h>

class Number_list : public Node {

    public:
        using Node::Node;

        Number_list(std::string str, U64 hash, int num_nums_in_list, bool is_angle = false):
            Node(str, hash)
        {   
            if (is_angle){
                constraint = std::make_optional<Size_constraint>(Common::angle_list, num_nums_in_list);
            } else {
                constraint = std::make_optional<Size_constraint>(Common::float_list, num_nums_in_list);
            }
        }

    private:

};

#endif
