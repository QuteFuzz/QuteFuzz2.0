#include <node.h>

int Node::node_counter = 0;

std::shared_ptr<Node> Node::find(const U64 _hash) const {

    std::shared_ptr<Node> node;

    for(const std::shared_ptr<Node>& child : children){
        if(*child == _hash){
            return child;
        }

        node = child->find(_hash);
        
        if(node != nullptr){
            return node;
        }
    }

    return nullptr;
}
