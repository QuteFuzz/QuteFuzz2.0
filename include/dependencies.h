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
        Node_dependencies(U64 comp){
            completer_hash = comp;
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

        void untrack_initiators(size_t n){
            if(n <= num_initiators) num_initiators -= n;
        }

        void increment_info(){
            info++;
            info_set = true;
        }

        int get_info(){return info;}

        bool is_info_set(){return info_set;}

        void reset(size_t num_init = 0){
            if(!num_init) num_init = initiator_hashes.size();
            else if(num_init > initiator_hashes.size()) num_init = initiator_hashes.size();

            num_initiators = num_init;
            info_set = false;
            info = 0;
        }

    private:
        std::vector<U64> initiator_hashes;
        size_t num_initiators = 0;
        U64 completer_hash = 0ULL;
        bool info_set = false;
        int info = 0;
};

#endif


