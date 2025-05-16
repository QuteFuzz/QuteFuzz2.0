#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "term.h"

/// @brief A constraint on some property of a branch
namespace Constraints {
    enum Type {
        NUM_RULES_MINIMUM = 1,
        NUM_RULES_MAXIMUM,
        NUM_RULES_EQUALS,
        BRANCH_IS_NON_RECURSIVE,
        BRANCH_EQUALS,
        BRANCH_IN,
    };

    struct Constraint {
        U64 node = 0; // constraint on a branch from a particular node
        Type type;
        std::variant<size_t, std::vector<U64>> value;
        bool satisfied_once = false;
        bool global = false;

        Constraint(Type t, bool _global = false){
            global = _global;

            if(t == BRANCH_IS_NON_RECURSIVE){
                type = t;
            }
        }

        Constraint(U64 _node, Type t, size_t val, bool _global = false) {
            node = _node;
            value = val;
            global = _global;

            if((t == NUM_RULES_MAXIMUM) || (t == NUM_RULES_MINIMUM) || (t == NUM_RULES_EQUALS)){
                type = t;

            }
        }

        Constraint(U64 _node, Type t, std::vector<U64> node_hashes, bool _global = false){
            node = _node;
            value = node_hashes;
            global = _global;

            if((t == BRANCH_EQUALS) || (t == BRANCH_IN)){
                type = t;

            }
        }

        /// @brief TODO: re-think constraint satisfaction and deletion
        /// @return 
        bool can_delete_constraint(){return true;}

        bool not_relevant(const U64 _node) const {return _node != node;}

        bool is_satisfied(const U64 _node, const Branch& b) const {

            size_t pts_size = b.num_pointer_terms();

            switch(type){
                case NUM_RULES_MAXIMUM: return not_relevant(_node) || (pts_size <= std::get<size_t>(value));
                case NUM_RULES_MINIMUM: return not_relevant(_node) || (pts_size >= std::get<size_t>(value));
                case NUM_RULES_EQUALS: return not_relevant(_node)  || (pts_size == std::get<size_t>(value));
                case BRANCH_IS_NON_RECURSIVE: return !b.get_recursive_flag();
                case BRANCH_EQUALS: return not_relevant(_node) || b.pointer_terms_match(std::get<std::vector<U64>>(value));
                case BRANCH_IN: return not_relevant(_node) || b.pointer_terms_in(std::get<std::vector<U64>>(value));
                default: return true;
            }

            return false;
        }

    };

    struct Constraints {
        
        public:
            Constraints(){}

            /// @brief TODO: Re-think constraint satisfaction and deletion
            /// @param b 
            /// @return 
            bool are_satisfied(const U64 _node, const Branch& b) const {
                
                for(const Constraint& c : constraints){
                    if(!c.is_satisfied(_node, b)) return false;
                }
                return true;
            }

            void add_constraint(Constraint c){
                constraints.push_back(c);
            }

            /// @brief Add a constraint on the number of qubits that should be picked for a gate
            /// @param n number of qubits
            /// @param is_rotation whether or not the gate requires an argument
            void add_n_qubit_constrait(size_t n, bool is_rotation = false){
                
                if(is_rotation){
                    add_arg_constraint();
                
                }  

                constraints.push_back(Constraint(Common::qubit_list, NUM_RULES_EQUALS, n)); 
            }
            
            /// @brief Only removes constraints that have been satisfied once already and have not been marked as global
            void safe_clear(){
                constraints.erase(
                    std::remove_if(constraints.begin(), constraints.end(), [](Constraint& c){
                        return c.can_delete_constraint();
                    } ),  
                    constraints.end()
                );
            }
        
        private:
            void add_arg_constraint(){
                constraints.push_back(Constraint(Common::gate_application_kind, BRANCH_EQUALS, {Common::float_literal, Common::qubit_list})); 
            }

            std::vector<Constraint> constraints = {};

    };
}

#endif
