#ifndef SINGULAR_QUBIT_H
#define SINGULAR_QUBIT_H

#include <node.h>
#include <variable.h>

class Singular_qubit : public Node {

    public:
        /// @brief Dummy qubit
        Singular_qubit() :
            Node("Singular_qubit", hash_rule_name("singular_qubit"))
        {}

        Singular_qubit(Variable _name) : 
            Node("Singular_qubit", hash_rule_name("singular_qubit")),
            name(_name)
        {}

        void reset(){used = false;}

        void set_used(){used = true;}
        
        bool is_used(){return used;}

        inline std::shared_ptr<Variable> get_name() const {
            return std::make_shared<Variable>(name);
        }
        
    private:
        bool used = false;
        Variable name;
};


#endif
