#ifndef DISCARD_QUBIT_DEF_H
#define DISCARD_QUBIT_DEF_H

#include <node.h>
#include <qubit_definition.h>

class Discard_qubit_def : public Node {

    public:

        Discard_qubit_def(std::string str, U64 hash, std::shared_ptr<Qubit_definition::Qubit_definition> def):
            Node(str, hash)
        {
            if (def->is_register_def()) {
                constraint = std::make_optional<Size_constraint>(Common::discard_single_qubit, 0);
            } else {
                constraint = std::make_optional<Size_constraint>(Common::discard_qreg, 0);
            }
                
        }

    private:

};

#endif