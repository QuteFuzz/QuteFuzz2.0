#ifndef SINGULAR_RESOURCE_DEFINITION_H
#define SINGULAR_RESOURCE_DEFINITION_H

#include <node.h>
#include <singular_resource.h>
#include <collection.h>
#include <resource.h>

class Singular_resource_definition : public Node {

    public:

        /// @brief Dummy resource definition
        Singular_resource_definition() : 
            Node()
        {}

        Singular_resource_definition(Variable _name) : 
            Node("singular_resource_def", hash_rule_name("singular_resource_def")),
            name(_name)
        {}

        std::shared_ptr<Variable> get_name(){
            return std::make_shared<Variable>(name);
        }

    protected:
        Variable name;

};

class Singular_qubit_definition : public Singular_resource_definition {
    
    public:
        Singular_qubit_definition(Variable _name) : 
            Singular_resource_definition(
                _name
            )
        
        {}

        void make_resources(Collection<Resource::Qubit>& output, Resource::Scope scope) const {
            Singular_qubit singular_qubit(name);
            output.add(Resource::Qubit(singular_qubit, scope));
        }

    private:

};

class Singular_bit_definition : public Singular_resource_definition {
    
    public:
        Singular_bit_definition(Variable _name) : 
            Singular_resource_definition(
                _name
            )
        {}

        void make_resources(Collection<Resource::Bit>& output, Resource::Scope scope) const {
            Singular_bit singular_bit(name);
            output.add(Resource::Bit(singular_bit, scope));
        }

    private:

};

#endif

