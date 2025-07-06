#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "term.h"
#include "branch.h"

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

            explicit Constraint(Type t, bool _global = false, bool _must_satisfy = false);

            explicit Constraint(U64 _node, Type t, size_t val, U64 _count_node, bool _global = false, bool _must_satisfy = false);

            explicit Constraint(U64 _node, Type t, std::vector<U64> node_hashes, bool _global = false, bool _must_satisfy = false);

            bool is_satisfied(const U64 _node, const Branch& b);

            bool on(const U64 node_hash, Type t, size_t n, U64 _count_node);

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

            inline void set_global(bool flag){
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

            bool are_satisfied(const U64 _node, const Branch& b);

            void add_rules_constraint(U64 node_hash, Type t, size_t n, U64 _count_node);

            void add_n_qubit_constraint(size_t n, bool is_rotation = false);

            inline void add_recursion_constraint(){ constraints[4].must_satisfy = true; }

            inline void allow_subroutines(bool flag){ constraints[7].set_global(!flag); }

            inline void set_qubit_kind(bool from_register) { 
                
                if(from_register){
                    constraints[8].must_satisfy = true;
                    constraints[9].must_satisfy = false;
                } else {
                    constraints[8].must_satisfy = false;
                    constraints[9].must_satisfy = true;
                }
                
            }
            
            /// @brief Prints just the default constraints
            /// @param stream 
            /// @param constraints 
            /// @return 
            friend std::ostream& operator<<(std::ostream& stream, Constraints constraints){
                for(int i = 0; i < 8; i++){
                    stream << constraints.constraints[i] << std::endl;

                }
                return stream;
            }
    
        private:
    
            std::vector<Constraint> constraints = {
                N_QUBIT_CONSTRAINT(1, false),
                N_QUBIT_CONSTRAINT(2, false),
                N_QUBIT_CONSTRAINT(3, false),
                Constraint(Common::gate_application_kind, BRANCH_EQUALS, {Common::float_literals, Common::qubit_list}),
                Constraint(Common::gate_application_kind, BRANCH_EQUALS, std::vector<U64>({Common::qubit_list})),
                Constraint(BRANCH_IS_NON_RECURSIVE),
                Constraint(Common::gate_name, BRANCH_IN, std::vector<U64>({Common::ry}), false),
                Constraint(Common::gate_application, BRANCH_EQUALS, {Common::circuit_name, Common::gate_name, Common::gate_application_kind}, true),
                Constraint(Common::qubit, BRANCH_EQUALS, {Common::qubit_name, Common::qubit_index}),
                Constraint(Common::qubit, BRANCH_EQUALS, std::vector<U64>({Common::qubit_name})),
            };

    };
}

#endif
