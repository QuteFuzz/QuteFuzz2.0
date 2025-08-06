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
        Singular_resource(Variable _name, bool _is_qubit, bool _owned) : 
            Node(_is_qubit ? "singular_qubit" : "singular_bit", 
                 hash_rule_name(_is_qubit ? "singular_qubit" : "singular_bit")),
            name(_name),
            is_qubit(_is_qubit),
            owned(_owned)
        {}

        void reset(){used = false;}

        void set_used(){used = true;}
        
        bool is_used(){return used;}

        bool get_resource_classification() {return is_qubit;}

        void set_owned(bool _owned) {
            owned = _owned;
        }

        bool is_owned() const {
            return owned;
        }

        inline std::shared_ptr<Variable> get_name() const {
            return std::make_shared<Variable>(name);
        }
        
    private:
        Variable name;
        bool used = false;
        bool is_qubit = true;
        bool owned = false;
        
};




#endif
