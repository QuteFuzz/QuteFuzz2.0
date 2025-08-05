#ifndef SINGULAR_RESOURCE_H
#define SINGULAR_RESOURCE_H

#include <node.h>
#include <variable.h>

namespace Singular_resource {
    
    class Singular_qubit : public Node {

        public:
            /// @brief Dummy qubit
            Singular_qubit() :
                Node("Singular_qubit", hash_rule_name("singular_qubit"))
            {}

            Singular_qubit(Variable _name) : 
                Node("Singular_qubit", hash_rule_name("singular_qubit")),
                name(_name)
            {}

            void reset(){used = false;}

            void set_used(){used = true;}
            
            bool is_used(){return used;}

            inline std::shared_ptr<Variable> get_name() const {
                return std::make_shared<Variable>(name);
            }
            
        private:
            bool used = false;
            Variable name;
    };

    class Singular_bit : public Node {
        public:

            /// @brief Dummy bit
            Singular_bit() :
                Node("Singular_bit", hash_rule_name("singular_bit"))
            {}

            Singular_bit(Variable _name) :
                Node("Singular_bit", hash_rule_name("singular_bit")),
                name(_name)
            {}

            void reset(){used = false;}

            void set_used(){used = true;}
            
            bool is_used(){return used;}

            inline std::shared_ptr<Variable> get_name() const {
                return std::make_shared<Variable>(name);
            }

        private:
            bool used = false;
            Variable name;
    };
}




#endif
