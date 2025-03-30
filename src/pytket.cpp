#include "../include/pytket.h"

std::ofstream& Pytket::Pytket::write_imports(std::ofstream& stream){
    stream << "from sympy import Symbol \n" <<
        "from helpers.pytket_helpers import test_circuit_on_passes \n" <<
        "from pathlib import Path \n" <<
        "from pytket import Circuit, Qubit, Bit \n" <<
        "from pytket.circuit import Op, OpType, MultiplexorBox"
    << std::endl;

    return stream;
}

std::ofstream& Pytket::Pytket::write(std::ofstream& stream, const Node& node) {

    if(node.is_terminal()){
        stream << node.get_value();
        return stream;
    }

    switch(hash_rule_name(node.get_value())){

        case GATE_NAME:
            stream << "\"";
            write_children(stream, node.get_children());
            stream << "\"";

            break;

        default:
            write_children(stream, node.get_children());
    }

    return stream;
}

void Pytket::Pytket::write(fs::path& path) {
    Node ast_root = build(); 
    
    std::ofstream stream(path.string());

    write_imports(stream) << std::endl;

    write(stream, build());

    std::cout << ast_root << std::endl;
    std::cout << "Written to " << path.string() << std::endl;
};