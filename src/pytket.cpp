#include "../include/pytket.h"

std::ofstream& Pytket::write(std::ofstream& stream, const Node& node) {
    stream << node.get_value() << std::endl;

    return stream;
}

