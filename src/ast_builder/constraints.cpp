#include "../../include/ast_builder/constraints.h"

namespace Constraints {

    Constraint::Constraint(Type t, bool _global, bool _must_satisfy){
        global = _global;
        must_satisfy = (global ? true : _must_satisfy);

        if(t == BRANCH_IS_NON_RECURSIVE){
            type = t;                
        } else {
            ERROR("BRANCH_IS_NON_RECURSIVE constraint type expected");
        }

    }

    Constraint::Constraint(U64 _node, Type t, size_t val, U64 _count_node, bool _global, bool _must_satisfy) {
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

    Constraint::Constraint(U64 _node, Type t, std::vector<U64> node_hashes, bool _global, bool _must_satisfy){
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

    bool Constraint::is_satisfied(const U64 _node, const Branch& b) {
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
            }


            return satisfied;

        } else {
            return true;
        }
    }

    bool Constraint::is(const U64 node_hash, Type t, size_t n, U64 _count_node){
        if((t == NUM_GIVEN_RULE_MAXIMUM) || (t == NUM_GIVEN_RULE_MINIMUM) || (t == NUM_GIVEN_RULE_EQUALS)){
            return (node_hash == node) && (n == std::get<size_t>(value)) && (count_node == _count_node);
        } else {
            ERROR("Should only call on for constraints counting number of rules in a branch");
            return false;
        }
    }

    bool Constraints::are_satisfied(const U64 _node, const Branch& b) {
        
        for(auto& constraint : constraints){
            if(!constraint.is_satisfied(_node, b)) return false;
        }
        return true;
    }

    /// @brief Add a constraint on the number of rules that should be picked for a node
    /// @param node_hash hash of the node to constrain
    /// @param t type of the constraint (NUM_GIVEN_RULE_MAXIMUM, NUM_GIVEN_RULE_MINIMUM, NUM_GIVEN_RULE_EQUALS)
    /// @param n number of rules for the constraint type
    /// @param _count_node node to count in chosen branch
    void Constraints::add_size_constraint(U64 node_hash, Type t, size_t n, U64 _count_node){

        if(n > WILDCARD_MAX){
            ERROR("Constraint on " + std::to_string(node_hash) + " for " + std::to_string(n) + " rules cannot be satisfied!");

        } else {

            for(Constraint& constraint : constraints){
                if (constraint.is(node_hash, t, n, _count_node)){
                    constraint.must_satisfy = true;
                    return;
                }
            }

            constraints.push_back(SIZE_CONSTRAINT(node_hash, t, n, _count_node, true));
            INFO("Constraint on " + std::to_string(node_hash) + " for " + std::to_string(n) + " rules added ");
    
        }
    }

    /// @brief Add a constraint on the number of qubits that should be picked for a gate
    /// @param n number of qubits
    /// @param is_rotation whether or not the gate requires an argument
    void Constraints::add_n_qubit_constraint(size_t n, bool is_rotation){

        if(is_rotation){
            constraints[3].must_satisfy = true;
            constraints[4].must_satisfy = false;
        } else {
            constraints[4].must_satisfy = true;
            constraints[3].must_satisfy = false;
        }

        for(Constraint& constraint : constraints){
            if(constraint.is(Common::qubit_list, NUM_GIVEN_RULE_EQUALS, n, Common::qubit)){
                constraint.must_satisfy = true;
                return;
            }
        }

        constraints.push_back(N_QUBIT_CONSTRAINT(n, true));
        INFO("Constraint for " + std::to_string(n) + " qubits added");
    }

}