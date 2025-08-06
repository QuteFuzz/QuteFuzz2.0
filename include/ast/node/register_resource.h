#ifndef REGISTER_RESOURCE_H
#define REGISTER_RESOURCE_H

#include <node.h>
#include <variable.h>
#include <integer.h>

class Register_resource : public Node {

    public:

        /// @brief Dummy resource
        Register_resource() :
            Node("Register_resource", hash_rule_name("register_resource"))
        {}

        Register_resource(Variable _name, Integer _index, bool _is_qubit, bool _owned) : 
            Node(_is_qubit ? "register_qubit" : "register_bit", 
                 hash_rule_name(_is_qubit ? "register_qubit" : "register_bit")),
            name(_name),
            index(_index),
            is_qubit(_is_qubit),
            owned(_owned)
        {}

        void reset(){used = false;}

        void set_used(){used = true;}
        
        bool is_used(){return used;}

        void set_owned(bool _owned) {
            owned = _owned;
        }

        bool is_owned() const {
            return owned;
        }

        bool get_resource_classification() const {return is_qubit;}

        inline std::shared_ptr<Variable> get_name() const {
            return std::make_shared<Variable>(name);
        }
        
        inline std::shared_ptr<Integer> get_index() const {
            return std::make_shared<Integer>(index);
        }

    private:
        Variable name;
        Integer index;
        bool is_qubit = true;
        bool used = false;
        bool owned = false;

};


#endif
