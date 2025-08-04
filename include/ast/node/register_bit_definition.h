#ifndef REGISTER_BIT_DEFINITION_H
#define REGISTER_BIT_DEFINITION_H

#include <node.h>
#include <register_bit.h>
#include <variable.h>
#include <integer.h>
#include <collection.h>

class Register_bit_definition : public Node {

    public:

        /// dummy bit def
        Register_bit_definition() : 
            Node("register_bit_def", hash_rule_name("register_bit_def"))
        {}

        Register_bit_definition(Variable _name, Integer _size): 
            Node("register_bit_def", hash_rule_name("register_bit_def")),
            name(_name),
            size(_size)
        {}

        std::shared_ptr<Variable> get_name(){
            return std::make_shared<Variable>(name);
        }

        std::shared_ptr<Integer> get_size(){
            return std::make_shared<Integer>(size);
        }

        /// @brief Add bits to the given vector from the rbit
        /// @param bits 
        void make_bits(Collection<Bit::Bit>& output, bool external) const {
            size_t reg_size = safe_stoi(size.get_string()).value();

            for(size_t i = 0; i < reg_size; i++){
                Register_bit reg_bit(name, Integer(std::to_string(i)));

                output.add(Bit::Bit(reg_bit, external));
            }
        }

    private:
        Variable name;
        Integer size;

};


#endif

