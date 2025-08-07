#ifndef SINGULAR_RESOURCE_H
#define SINGULAR_RESOURCE_H

#include <node.h>
#include <variable.h>

class Singular_resource : public Node {
    public:
        /// @brief Dummy resource
        Singular_resource() :
            Node("Singular_resource", hash_rule_name("singular_resource"))
        {}

        // Initialize singular resource with node and hash rule name based on input type
        Singular_resource(std::string str, U64 hash, Variable _name) : 
            Node(str, hash),
            name(_name)
        {}

        void reset(){used = false;}

        void set_used(){used = true;}
        
        bool is_used(){return used;}

        inline std::shared_ptr<Variable> get_name() const {
            return std::make_shared<Variable>(name);
        }
        
    private:
        Variable name;
        bool used = false;
};

class Singular_qubit : public Singular_resource {

    public:
        Singular_qubit(Variable _name):
            Singular_resource("singular_qubit", Common::singular_qubit ,_name)
        {}

    private:

};

class Singular_bit : public Singular_resource {

    public:
        Singular_bit(Variable _name):
            Singular_resource("singular_bit", Common::singular_bit, _name)
        {}

    private:

};


#endif
