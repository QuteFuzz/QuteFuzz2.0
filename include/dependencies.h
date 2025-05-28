#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

#include <vector>
#include "utils.h"

enum ND_Node_kind {
    ND_INIT,
    ND_COMP,
};


struct Node_dependencies {
    public:
        Node_dependencies(){}

        Node_dependencies(U64 comp){
            completer_hash = comp;
        }

        Node_dependencies(U64 init, U64 comp){
            completer_hash = comp;
            initiator_hashes.push_back(init);
            num_initiators ++;
        }

        void add_initiator(U64 init){
            if(!node_is(ND_INIT, init)){
                initiator_hashes.push_back(init);
                num_initiators ++;
            }
        }

        bool node_is(ND_Node_kind kind, U64 node_hash) const {
            switch(kind){
                case ND_INIT: return std::find(initiator_hashes.begin(), initiator_hashes.end(), node_hash) != initiator_hashes.end();
                case ND_COMP: return completer_hash == node_hash;
                default: return false;
            }
        }

        bool no_outstanding_dependencies(){return (num_initiators == 0);}

        void untrack_initiator(){
            num_initiators -= 1;
        }

        void increment_info(){
            info++;
            info_set = true;
        }

        int get_info(){return info;}

        bool is_info_set(){return info_set;}

        void reset(){
            num_initiators = initiator_hashes.size();
            info_set = false;
            info = 0;
        }

        Node_dependencies get_subset(U64 initiator_hash){

            if(node_is(ND_INIT, initiator_hash)){
                return Node_dependencies(initiator_hash, completer_hash);
            } else {
                return Node_dependencies(completer_hash);
            }

        }

        friend std::ostream& operator<<(std::ostream& stream, Node_dependencies nds){
            stream << "Initiators" << std::endl;
            for(U64 hash : nds.initiator_hashes){
                stream << hash << std::endl;
            }

            stream << "completer " << nds.completer_hash << std::endl;

            return stream;
        }

    private:
        std::vector<U64> initiator_hashes;
        size_t num_initiators = 0;
        U64 completer_hash = 0ULL;
        bool info_set = false;
        int info = 0;
};

#endif


