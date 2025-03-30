#include "../include/pytket.h"

std::ofstream& Pytket::write_imports(std::ofstream& stream){
    stream << "from sympy import Symbol \n" <<
        "from helpers.pytket_helpers import test_circuit_on_passes \n" <<
        "from pathlib import Path \n" <<
        "from pytket import Circuit, Qubit, Bit \n" <<
        "from pytket.circuit import Op, OpType, MultiplexorBox"
    << std::endl;

    return stream;
}

std::ofstream& Pytket::write(std::ofstream& stream, const Node& node) {

    if(node.is_terminal()){
        stream << node.get_value();
    } else {
        std::cout << node.get_value() << " " << hash_rule_name(node.get_value()) << std::endl;
    }

    for(auto child : node.get_children()){
        write(stream, *child);
    }

    return stream;
}

void Pytket::write(fs::path& path) {
    Node ast_root = build(); 
    
    std::ofstream stream(path.string());

    write_imports(stream) << std::endl;

    write(stream, build());

    std::cout << ast_root << std::endl;
    std::cout << "Written to " << path.string() << std::endl;
};