#ifndef ARG_H
#define ARG_H

#include <node.h>

namespace Arg {

    enum Type {
        SINGULAR,
        REGISTER,
        //INTEGER,
        //FLOAT,
    };

    class Arg : public Node {
        public:

            /// @brief Dummy argument
            Arg() :
                Node("arg", hash_rule_name("arg")),
                type(SINGULAR)
            {
                constraint = std::make_optional<Node_constraint>(Common::qubit, 1);
            }

            Arg(const std::string& str, const U64& hash, Type _type) :
                Node(str, hash),
                type(_type)
            {
                if (type == SINGULAR) {
                    constraint = std::make_optional<Node_constraint>(Common::arg_singular_qubit, 1);
                } else if (type == REGISTER) {
                    constraint = std::make_optional<Node_constraint>(Common::arg_register_qubits, 1);
                }
            }
            
        private:
            Type type;

    };

}

#endif
