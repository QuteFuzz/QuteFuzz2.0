#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "term.h"

/// @brief A constraint on some property of a branch
namespace Constraints {
    enum Type {
        NUM_GIVEN_RULE_MINIMUM = 1,
        NUM_GIVEN_RULE_MAXIMUM,
        NUM_GIVEN_RULE_EQUALS,
        BRANCH_IS_NON_RECURSIVE,
        BRANCH_EQUALS,
        BRANCH_IN,
    };

    struct Constraint {

        public:
            bool must_satisfy;

            explicit Constraint(Type t, bool _global = false, bool _must_satisfy = false){
                global = _global;
                must_satisfy = (global ? true : _must_satisfy);

                if(t == BRANCH_IS_NON_RECURSIVE){
                    type = t;                
                } else {
                    ERROR("BRANCH_IS_NON_RECURSIVE constraint type expected");
                }

            }

            explicit Constraint(U64 _node, Type t, size_t val, U64 _count_node, bool _global = false, bool _must_satisfy = false) {
                node = _node;
                value = val;
                global = _global;
                must_satisfy = (global ? true : _must_satisfy);
                count_node = _count_node;

                if((t == NUM_GIVEN_RULE_MAXIMUM) || (t == NUM_GIVEN_RULE_MINIMUM) || (t == NUM_GIVEN_RULE_EQUALS)){
                    type = t;                
                } else {
                    ERROR("NUM_GIVEN_RULE_MAXIMUM, NUM_GIVEN_RULE_MINIMUM, NUM_GIVEN_RULE_EQUALS constraint types expected");
                }
            }

            explicit Constraint(U64 _node, Type t, std::vector<U64> node_hashes, bool _global = false, bool _must_satisfy = false){
                node = _node;
                value = node_hashes;
                global = _global;
                must_satisfy = (global ? true : _must_satisfy);

                if((t == BRANCH_EQUALS) || (t == BRANCH_IN)){
                    type = t;                
                } else {
                    ERROR("BRANCH_EQUALS, BRANCH_IN constraint types expected");
                }
            }

            bool is_satisfied(const U64 _node, const Branch& b) {
                bool constraint_on_this_node = (_node == node);

                if(must_satisfy && constraint_on_this_node){
                    size_t pts_size = b.num_pointer_terms(count_node);
                    bool satisfied = false;
                    
                    switch(type){ 
                        case NUM_GIVEN_RULE_MAXIMUM: satisfied = pts_size <= std::get<size_t>(value); break;
                        case NUM_GIVEN_RULE_MINIMUM: satisfied = pts_size >= std::get<size_t>(value); break;
                        case NUM_GIVEN_RULE_EQUALS: satisfied = pts_size == std::get<size_t>(value); break;
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

            bool on(const U64 node_hash, Type t, size_t n, U64 _count_node){
                if((t == NUM_GIVEN_RULE_MAXIMUM) || (t == NUM_GIVEN_RULE_MINIMUM) || (t == NUM_GIVEN_RULE_EQUALS)){
                    return (node_hash == node) && (n == std::get<size_t>(value)) && (count_node == _count_node);
                } else {
                    ERROR("Should only call on for rule constraints");
                    return false;
                }
            }

            friend std::ostream& operator<<(std::ostream& stream, Constraint& constraint){
                stream << "============================================" << std::endl;
                stream << "on: " << constraint.node << " " << std::endl;

                switch(constraint.type){ 
                    case NUM_GIVEN_RULE_MAXIMUM: stream << "NUM_GIVEN_RULE_MAXIMUM " << std::get<size_t>(constraint.value) << " counting " << constraint.count_node;  break;
                    case NUM_GIVEN_RULE_MINIMUM: stream << "NUM_GIVEN_RULE_MINIMUM " << std::get<size_t>(constraint.value) << " counting " << constraint.count_node;  break;
                    case NUM_GIVEN_RULE_EQUALS: stream << "NUM_GIVEN_RULE_EQUALS " << std::get<size_t>(constraint.value) << " counting " << constraint.count_node; break;
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

                stream << " must satisfy: " << constraint.must_satisfy << " global: " << constraint.global << std::endl; 
                stream << "============================================" << std::endl;

                return stream;
            }

            void set_global(bool flag){
                global = flag; 
                must_satisfy = flag;
            }

        private:
            U64 node = 0; // constraint on a branch from a particular node
            U64 count_node = 0; // node to count in chosen branch
            Type type;
            std::variant<size_t, std::vector<U64>> value;
            bool global = false;

    };

    #define ON_RULES_CONSTRAINT(node_hash, type, n, count_node, must_satisfy) (Constraint(node_hash, type, n, count_node, false, must_satisfy))
    #define N_QUBIT_CONSTRAINT(n, must_satisfy) (ON_RULES_CONSTRAINT(Common::qubit_list, NUM_GIVEN_RULE_EQUALS, n, Common::qubit, must_satisfy))

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
            void add_rules_constraint(U64 node_hash, Type t, size_t n, U64 _count_node){

                for(Constraint& constraint : constraints){
                    if (constraint.on(node_hash, t, n, _count_node)){
                        constraint.must_satisfy = true;
                        return;
                    }
                }

                constraints.push_back(ON_RULES_CONSTRAINT(node_hash, t, n, _count_node, true));
                INFO("Constraint on " + std::to_string(node_hash) + " for " + std::to_string(n) + " rules added ");
            }

            /// @brief Add a constraint on the number of qubits that should be picked for a gate
            /// @param n number of qubits
            /// @param is_rotation whether or not the gate requires an argument
            void add_n_qubit_constraint(size_t n, bool is_rotation = false){

                if(is_rotation){
                    constraints[3].must_satisfy = true;
                } else {
                    constraints[4].must_satisfy = true;
                }

                for(Constraint& constraint : constraints){
                    if(constraint.on(Common::qubit_list, NUM_GIVEN_RULE_EQUALS, n, Common::qubit)){
                        constraint.must_satisfy = true;
                        return;
                    }
                }

                constraints.push_back(N_QUBIT_CONSTRAINT(n, true));
                INFO("Constraint for " + std::to_string(n) + " qubits added");
            }

            void add_recursion_constraint(){
                constraints[4].must_satisfy = true;
            }

            void allow_subroutines(bool flag){
                constraints[7].set_global(!flag);
            }

            friend std::ostream& operator<<(std::ostream& stream, Constraints constraints){
                for(auto& c : constraints.constraints){
                    stream << c << std::endl;
                }

                return stream;
            }
    
        private:
    
            std::vector<Constraint> constraints = {
                N_QUBIT_CONSTRAINT(1, false),
                N_QUBIT_CONSTRAINT(2, false),
                N_QUBIT_CONSTRAINT(3, false),
                Constraint(Common::gate_application_kind, BRANCH_EQUALS, {Common::float_literal, Common::qubit_list}),
                Constraint(Common::gate_application_kind, BRANCH_EQUALS, std::vector<U64>({Common::qubit_list})),
                Constraint(BRANCH_IS_NON_RECURSIVE),
                // Constraint(Common::gate_name, BRANCH_IN, {Common::h, Common::x}, true),
                Constraint(Common::gate_name, BRANCH_IN, {Common::cx, Common::cz, Common::cnot}, true),
                Constraint(Common::gate_application, BRANCH_EQUALS, {Common::circuit_name, Common::gate_name, Common::gate_application_kind}, true),
            };

    };
}

#endif
