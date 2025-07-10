#ifndef REGISTER_QUBIT_H
#define REGISTER_QUBIT_H

#include <node.h>
#include <variable.h>
#include <integer.h>

class Register_qubit : public Node {

    public:

        /// @brief Dummy qubit
        Register_qubit() :
            Node("Register_qubit", hash_rule_name("register_qubit"))
        {}

        Register_qubit(Variable _name, Integer _index) : 
            Node("Register_qubit", hash_rule_name("register_qubit")),
            name(_name),
            index(_index)
        {}

        void reset(){used = false;}

        void set_used(){used = true;}
        
        bool is_used(){return used;}

        inline std::shared_ptr<Variable> get_name(){
            return std::make_shared<Variable>(name);
        }
        
        inline std::shared_ptr<Integer> get_index(){
            return std::make_shared<Integer>(index);
        }

    private:
        Variable name;
        Integer index;
        bool used = false;

};


#endif
