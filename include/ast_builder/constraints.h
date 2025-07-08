#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "../grammar_parser/term.h"
#include "../grammar_parser/branch.h"
#include "qubit_def.h"

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

            bool is(const U64 node_hash, Type t, size_t n, U64 _count_node);

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

    #define SIZE_CONSTRAINT(node_hash, type, n, count_node, must_satisfy) (Constraint(node_hash, type, n, count_node, false, must_satisfy))
    #define N_QUBIT_CONSTRAINT(n, must_satisfy) (SIZE_CONSTRAINT(Common::qubit_list, NUM_GIVEN_RULE_EQUALS, n, Common::qubit, must_satisfy))

    struct Constraints {
        
        public:
            Constraints(){
                num_default_constraints = constraints.size();
            }

            bool are_satisfied(const U64 _node, const Branch& b);

            void push_back(const Constraint& constraint){
                constraints.push_back(constraint);
            }

            void add_n_qubit_constraint(size_t n, bool is_rotation = false);

            inline void add_recursion_constraint(){ 
                constraints[4].must_satisfy = true; 
            }

            inline void allow_subroutines(bool flag){ 
                constraints[7].set_global(!flag); 
            }

            void constrain_qubit_def(Qubit_def::Type type){
                if(type == Qubit_def::REGISTER){
                    constraints[8].must_satisfy = true;
                    constraints[9].must_satisfy = true;
                    constraints[10].must_satisfy = true;
                    constraints[11].must_satisfy = false;
                    constraints[12].must_satisfy = false;
                    constraints[13].must_satisfy = false;
                } else {
                    constraints[8].must_satisfy = false;
                    constraints[9].must_satisfy = false;
                    constraints[10].must_satisfy = false;
                    constraints[11].must_satisfy = true;
                    constraints[12].must_satisfy = true;
                    constraints[13].must_satisfy = true;
                }
            }

            void constrain_qubit(bool from_register){
                if(from_register){
                    constraints[14].must_satisfy = true;
                    constraints[15].must_satisfy = false;
                } else {
                    constraints[14].must_satisfy = false;
                    constraints[15].must_satisfy = true;
                }
            }

            void constrain_num_subroutines(){
                constraints[16].must_satisfy = true;
            }

            void add_size_constraint(U64 node_hash, Type t, size_t n, U64 _count_node);
            
            /// @brief Prints just the default constraints
            /// @param stream 
            /// @param constraints 
            /// @return 
            friend std::ostream& operator<<(std::ostream& stream, Constraints constraints){
                for(size_t i = 0; i < constraints.num_default_constraints; i++){
                    stream << constraints.constraints[i] << std::endl;

                }
                return stream;
            }
    
        private:
    
            std::vector<Constraint> constraints = {
                /*
                    0, 1, 2, constraints for up to 3 qubits, others added during runtime
                */
                N_QUBIT_CONSTRAINT(1, false),
                N_QUBIT_CONSTRAINT(2, false),
                N_QUBIT_CONSTRAINT(3, false),

                /*
                    3, 4, constraints on gate applications with / without arguments
                */
                Constraint(Common::gate_application_kind, BRANCH_EQUALS, {Common::float_literals, Common::qubit_list}),
                Constraint(Common::gate_application_kind, BRANCH_EQUALS, std::vector<U64>({Common::qubit_list})),

                /*
                    5, choose branch without reference to the rule
                */
                Constraint(BRANCH_IS_NON_RECURSIVE),

                /*
                    6, Gate subset for swarm testing
                */
                Constraint(Common::gate_name, BRANCH_IN, std::vector<U64>({Common::ry}), false),
                
                /*
                    7, Allow subroutines, by choosing only branch containing gate name and not subroutine names
                */
                Constraint(Common::gate_application, BRANCH_IN, {Common::gate_name, Common::gate_application_kind}, true),

                /*
                    8, 9, 10, 11, 12, 13 qubit definition is singular, or a register
                */
                Constraint(Common::qubit_def, BRANCH_EQUALS, std::vector<U64>({Common::qubit_def_register})),
                Constraint(Common::internal_qubit_def, BRANCH_EQUALS, std::vector<U64>({Common::internal_qubit_def_register})),
                Constraint(Common::external_qubit_def, BRANCH_EQUALS, std::vector<U64>({Common::external_qubit_def_register})),
                Constraint(Common::qubit_def, BRANCH_EQUALS, std::vector<U64>({Common::qubit_def_singular})),
                Constraint(Common::internal_qubit_def, BRANCH_EQUALS, std::vector<U64>({Common::internal_qubit_def_singular})),
                Constraint(Common::external_qubit_def, BRANCH_EQUALS, std::vector<U64>({Common::external_qubit_def_singular})),
                

                /*
                    14, 15, qubit application is singular or from register
                */
                Constraint(Common::qubit, BRANCH_EQUALS, std::vector<U64>({Common::register_qubit_apply})),
                Constraint(Common::qubit, BRANCH_EQUALS, std::vector<U64>({Common::singular_qubit_apply})),

                /*
                    16, how many subroutines should be generated?
                */
                Constraint(Common::subroutines, NUM_GIVEN_RULE_MAXIMUM, Common::MAX_SUBROUTINES, Common::block)
            };

            size_t num_default_constraints;

    };
}

#endif
