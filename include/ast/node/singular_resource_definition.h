#ifndef SINGULAR_RESOURCE_DEFINITION_H
#define SINGULAR_RESOURCE_DEFINITION_H

#include <node.h>
#include <resource.h>

namespace Singular_resource_definition {

    class Singular_qubit_definition : public Node {

        public:

            /// dummy qubit def
            Singular_qubit_definition() : 
                Node("singular_qubit_def", hash_rule_name("singular_qubit_def"))
            {}

            Singular_qubit_definition(Variable _name) : 
                Node("singular_qubit_def", hash_rule_name("singular_qubit_def")),
                name(_name)
            {}

            std::shared_ptr<Variable> get_name(){
                return std::make_shared<Variable>(name);
            }

            void make_qubits(Collection<Resource::Resource>& output, bool external) const {
                Singular_resource::Singular_qubit singular_qubit(name);
                output.add(Resource::Resource(singular_qubit, external));
            }

        private:
            Variable name;

    };

    class Singular_bit_definition : public Node {

        public:

            /// dummy bit def
            Singular_bit_definition() : 
                Node("singular_bit_def", hash_rule_name("singular_bit_def"))
            {}

            Singular_bit_definition(Variable _name) : 
                Node("singular_bit_def", hash_rule_name("singular_bit_def")),
                name(_name)
            {}

            std::shared_ptr<Variable> get_name(){
                return std::make_shared<Variable>(name);
            }

            void make_bits(Collection<Resource::Resource>& output, bool external) const {
                Singular_resource::Singular_bit singular_bit(name);
                output.add(Resource::Resource(singular_bit, external));
            }

        private:
            Variable name;

    };
}




#endif

