#ifndef ARG_H
#define ARG_H

#include <node.h>

class Arg : public Node {
    public:

        /// @brief Dummy argument
        Arg() :
            Node("arg", Common::arg)
        {}

        Arg(std::shared_ptr<Resource_definition> qubit_def):
            Node("arg", Common::arg)
        {
            if(!qubit_def->is_register_def()){
                add_constraint(Common::qubit, 1);
                qubit_def_size = 1;
            
            } else {
                add_constraint(Common::qubit_list, 1);
                qubit_def_size = std::stoi(qubit_def->get_size()->get_string());
            }

        }

        size_t get_qubit_def_size(){
            return qubit_def_size;
        }
        
    private:
        size_t qubit_def_size = 0;

};

#endif
