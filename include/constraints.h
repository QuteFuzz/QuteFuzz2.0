#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "term.h"

/// @brief A constraint on some property of a branch
namespace Constraints {
    enum Type {
        NUM_RULES_MINIMUM,
        NUM_RULES_MAXIMUM,
        NUM_RULES_EQUALS,
        BRANCH_IS_NON_RECURSIVE,
        BRANCH_EQUALS,
    };

    struct Constraint {
        uint64_t node = 0; // constraint on a branch from a particular node
        Type type;
        std::variant<size_t, std::vector<uint64_t>> value;

        Constraint(Type t){
            if(t == BRANCH_IS_NON_RECURSIVE){
                type = t;
            }
        }

        Constraint(uint64_t _node, Type t, size_t val) {
            node = _node;
            value = val;

            if((t == NUM_RULES_MAXIMUM) || (t == NUM_RULES_MINIMUM) || (t == NUM_RULES_EQUALS)){
                type = t;

            }
        }

        Constraint(uint64_t _node, Type t, std::vector<uint64_t> node_hashes){
            node = _node;
            value = node_hashes;

            if(t == BRANCH_EQUALS){
                type = t;

            }
        }

        bool not_relevant(const uint64_t _node) const {return _node != node;}

        bool is_satisfied(const uint64_t _node, const Branch& b) const {

            size_t pts_size = b.num_pointer_terms();

            switch(type){
                case NUM_RULES_MAXIMUM: return not_relevant(_node) || (pts_size <= std::get<size_t>(value));
                case NUM_RULES_MINIMUM: return not_relevant(_node) || (pts_size >= std::get<size_t>(value));
                case NUM_RULES_EQUALS: return not_relevant(_node)  || (pts_size == std::get<size_t>(value));
                case BRANCH_IS_NON_RECURSIVE: return !b.get_recursive_flag();
                case BRANCH_EQUALS: return not_relevant(_node) || b.pointer_terms_match(std::get<std::vector<uint64_t>>(value));
            }

            return false;
        }

    };

    struct Constraints {
        
        public:
            Constraints(){}

            /// @brief Check that all constraints on this branch are satisfied
            /// @param b 
            /// @return 
            bool are_satisfied(const uint64_t _node, const Branch& b) const {
                
                for(const Constraint& c : constraints){
                    if(!c.is_satisfied(_node, b)) return false;
                }
                return true;
            }

            void add_constraint(Constraint c){
                constraints.push_back(c);
            }

            void clear(){
                constraints.clear();
            }
        
        private:
            std::vector<Constraint> constraints = {};

    };
}

#endif
