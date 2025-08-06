#ifndef SINGULAR_RESOURCE_DEFINITION_H
#define SINGULAR_RESOURCE_DEFINITION_H

#include <node.h>
#include <singular_resource.h>
#include <collection.h>

class Singular_resource_definition : public Node {

    public:

        /// @brief Dummy resource definition
        Singular_resource_definition() : 
            Node("Singular_resource_definition", hash_rule_name("singular_resource_definition"))
        {}

        Singular_resource_definition(Variable _name, bool _is_qubit, bool _owned) : 
            Node(_is_qubit ? "singular_qubit_def" : "singular_bit_def", 
                 hash_rule_name(_is_qubit ? "singular_qubit_def" : "singular_bit_def")),
            name(_name),
            resource_type(_is_qubit ? Resource::QUBIT : Resource::BIT),
            owned(_owned)
        {}

        std::shared_ptr<Variable> get_name(){
            return std::make_shared<Variable>(name);
        }

        bool is_owned() const {
            return owned;
        }

        void set_owned(bool _owned) {
            owned = _owned;
        }

        /// @brief Add resources to the given vector
        /// @param output 
        void make_resources(Collection<Resource::Resource>& output, bool external, bool _is_qubit, bool _owned) const {
            Singular_resource singular_resource(name, _is_qubit, _owned);
            output.add(Resource::Resource(singular_resource, external, _owned));
        }

    private:
        Variable name;
        Resource::Resource_Classification resource_type;
        bool owned = false;

};




#endif

