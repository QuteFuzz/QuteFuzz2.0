#ifndef REGISTER_BIT_H
#define REGISTER_BIT_H

#include <node.h>
#include <variable.h>
#include <integer.h>

class Register_bit : public Node {

    public:

        /// @brief Dummy bit
        Register_bit() :
            Node("Register_bit", hash_rule_name("register_bit"))
        {}

        Register_bit(Variable _name, Integer _index) : 
            Node("Register_bit", hash_rule_name("register_bit")),
            name(_name),
            index(_index)
        {}

        void reset(){used = false;}

        void set_used(){used = true;}
        
        bool is_used(){return used;}

        inline std::shared_ptr<Variable> get_name() const {
            return std::make_shared<Variable>(name);
        }
        
        inline std::shared_ptr<Integer> get_index() const {
            return std::make_shared<Integer>(index);
        }

    private:
        Variable name;
        Integer index;
        bool used = false;

};


#endif
