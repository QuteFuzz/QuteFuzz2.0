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

        public:
            bool must_satisfy;

            Constraint(Type t, bool _global = false){
                global = _global;
                must_satisfy = global;

                if(t == BRANCH_IS_NON_RECURSIVE){
                    type = t;
                }
            }

            Constraint(U64 _node, Type t, size_t val, bool _global = false) {
                node = _node;
                value = val;
                global = _global;
                must_satisfy = global;

                if((t == NUM_RULES_MAXIMUM) || (t == NUM_RULES_MINIMUM) || (t == NUM_RULES_EQUALS)){
                    type = t;

                }
            }

            Constraint(U64 _node, Type t, std::vector<U64> node_hashes, bool _global = false){
                node = _node;
                value = node_hashes;
                global = _global;
                must_satisfy = global;

                if((t == BRANCH_EQUALS) || (t == BRANCH_IN)){
                    type = t;

                }
            }

            bool is_satisfied(const U64 _node, const Branch& b) {
                bool constraint_on_this_node = (_node == node);

                if(must_satisfy && constraint_on_this_node){
                    size_t pts_size = b.num_pointer_terms();
                    bool satisfied = false;
                    
                    switch(type){ 
                        case NUM_RULES_MAXIMUM: satisfied = pts_size <= std::get<size_t>(value); break;
                        case NUM_RULES_MINIMUM: satisfied = pts_size >= std::get<size_t>(value); break;
                        case NUM_RULES_EQUALS: satisfied = pts_size == std::get<size_t>(value); break;
                        case BRANCH_IS_NON_RECURSIVE: satisfied = !b.get_recursive_flag(); break;
                        case BRANCH_EQUALS: satisfied = b.pointer_terms_match(std::get<std::vector<U64>>(value)); break;
                        case BRANCH_IN: satisfied = b.pointer_terms_in(std::get<std::vector<U64>>(value)); break;
                        default: satisfied = true; break;
                    }

                    if(satisfied && !global){
                        must_satisfy = false;
                        // std::cout << "Constraint satisfied" << std::endl;
                    }

                    // std::cout << "waiting" << std::endl;

                    return satisfied;

                } else {
                    return true;
                }
            }

            bool on(U64 node_hash, Type t, size_t n){
                if((t == NUM_RULES_MAXIMUM) || (t == NUM_RULES_MINIMUM) || (t == NUM_RULES_EQUALS)){
                    return (node_hash == node) && (n == std::get<size_t>(value));
                } else {
                    ERROR("Should only call on for rule constraints");
                    return false;
                }
            }

            friend std::ostream& operator<<(std::ostream& stream, Constraint& constraint){
                stream << "on: " << constraint.node << " ";

                switch(constraint.type){ 
                    case NUM_RULES_MAXIMUM: stream << "NUM_RULES_MAXIMUM " << std::get<size_t>(constraint.value); break;
                    case NUM_RULES_MINIMUM: stream << "NUM_RULES_MINIMUM " << std::get<size_t>(constraint.value); break;
                    case NUM_RULES_EQUALS: stream << "NUM_RULES_EQUALS " << std::get<size_t>(constraint.value); break;
                    case BRANCH_IS_NON_RECURSIVE: stream << "BRANCH_IS_NON_RECURSIVE "; break;
                    case BRANCH_EQUALS: {
                        stream << "BRANCH_EQUALS ";

                        for(const U64 hash : std::get<std::vector<U64>>(constraint.value)){
                            stream << hash << " ";
                        }    
                
                        break;
                    }
                    case BRANCH_IN: {
                        stream << "BRANCH_IN ";

                        for(const U64 hash : std::get<std::vector<U64>>(constraint.value)){
                            stream << hash << " ";
                        }    
                
                        break;
                    }
                    default: stream << "Unknown rule "; break;
                }

                stream << " must satisfy: " << constraint.must_satisfy << " global: " << constraint.global; 

                return stream;
            }

        private:
            U64 node = 0; // constraint on a branch from a particular node
            Type type;
            std::variant<size_t, std::vector<U64>> value;
            bool global = false;

    };

    #define ON_RULES_CONSTRAINT(node_hash, type, n) (Constraint(node_hash, type, n))
    #define N_QUBIT_CONSTRAINT(n) (ON_RULES_CONSTRAINT(Common::qubit_list, NUM_RULES_EQUALS, n))

    struct Constraints {
        
        public:
            Constraints(){}

            /// @brief TODO: Re-think constraint satisfaction and deletion
            /// @param b 
            /// @return 
            bool are_satisfied(const U64 _node, const Branch& b) {
                
                for(auto& constraint : constraints){
                    if(!constraint.is_satisfied(_node, b)) return false;
                }
                return true;
            }

            /// @brief This should be used for potentially new constraints on number of rules a branch should have. If the constraint already exists, it will be set for satisfaction.
            /// If not, a new one is added for that node and number of rules
            /// @param c 
            void add_rules_constraint(U64 node_hash, Type t, size_t n){
                bool found = false;

                for(auto& constraint : constraints){
                    if (constraint.on(node_hash, t, n)){
                        constraint.must_satisfy = true;
                        found = true;
                        break;
                    }
                }

                if(!found){
                    Constraint c = ON_RULES_CONSTRAINT(node_hash, t, n);
                    c.must_satisfy = true;
                    constraints.push_back(c);
                }
            }

            /// @brief Add a constraint on the number of qubits that should be picked for a gate
            /// @param n number of qubits
            /// @param is_rotation whether or not the gate requires an argument
            void add_n_qubit_constraint(size_t n, bool is_rotation = false){
                
                switch(n){
                    case 1: constraints[0].must_satisfy = true; break;
                    case 2: constraints[1].must_satisfy = true; break;
                    case 3: constraints[2].must_satisfy = true; break;
                    default: ERROR("Constraint for " + std::to_string(n) + " qubits not supported"); break;                
                }

                if(is_rotation){
                    constraints[3].must_satisfy = true;
                } else {
                    constraints[4].must_satisfy = true;
                }
            }

            void add_recursion_constraint(){
                constraints[4].must_satisfy = true;
            }

            friend std::ostream& operator<<(std::ostream& stream, Constraints constraints){
                for(auto& c : constraints.constraints){
                    stream << c << std::endl;
                }

                return stream;
            }
    
        private:
    
            std::vector<Constraint> constraints = {
                N_QUBIT_CONSTRAINT(1),
                N_QUBIT_CONSTRAINT(2),
                N_QUBIT_CONSTRAINT(3),
                Constraint(Common::gate_application_kind, BRANCH_EQUALS, {Common::float_literal, Common::qubit_list}),
                Constraint(Common::gate_application_kind, BRANCH_EQUALS, {Common::qubit_list}),
                Constraint(BRANCH_IS_NON_RECURSIVE),
                Constraint(Common::gate_name, BRANCH_IN, {Common::h, Common::x}, true),
            };

    };
}

#endif
