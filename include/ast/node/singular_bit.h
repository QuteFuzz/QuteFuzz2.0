#ifndef SINGULAR_BIT_H
#define SINGULAR_BIT_H

#include <node.h>
#include <variable.h>

class Singular_bit : public Node {

    public:
        /// @brief Dummy bit
        Singular_bit() :
            Node("Singular_bit", hash_rule_name("singular_bit"))
        {}

        Singular_bit(Variable _name) : 
            Node("Singular_bit", hash_rule_name("singular_bit")),
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
