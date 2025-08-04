#ifndef SINGULAR_BIT_DEFINITION_H
#define SINGULAR_BIT_DEFINITION_H

#include <node.h>

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

        void make_bits(Collection<Bit::Bit>& output, bool external) const {
            Singular_bit singular_bit(name);
            output.add(Bit::Bit(singular_bit, external));
        }

    private:
        Variable name;

};


#endif

