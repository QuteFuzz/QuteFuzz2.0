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

        Register_resource(std::string str, U64 hash, Variable _name, Integer _index) : 
            Node(str, hash),
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

class Register_qubit : public Register_resource {

    public:
        Register_qubit(Variable _name, Integer _index) : 
            Register_resource("register_qubit", Common::register_qubit, _name, _index)
        {}

    private:

};

class Register_bit : public Register_resource {

    public:
        Register_bit(Variable _name, Integer _index) : 
            Register_resource("register_bit", Common::register_bit, _name, _index)
        {}

    private:

};

#endif
