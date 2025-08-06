#ifndef ARG_H
#define ARG_H

#include <node.h>

class Arg : public Node {
    public:

        /// @brief Dummy argument
        Arg() :
            Node("arg", hash_rule_name("arg"))
        {}

        Arg(const std::string& str, const U64& hash, std::shared_ptr<Resource_definition> qubit_def):
            Node(str, hash)
        {
            if(qubit_def->get_type() == Resource::SINGULAR_EXTERNAL){
                constraint = std::make_optional<Node_constraint>(Common::arg_singular_qubit, 1);
                qubit_def_size = 1;
            
            } else {
                constraint = std::make_optional<Node_constraint>(Common::arg_register_qubits, 1);
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
