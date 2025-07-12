#ifndef BLOCK_H
#define BLOCK_H

#include <node.h>
#include <qubit.h>
#include <qubit_definition.h>

class Block : public Node {

    public:

        Block() : 
            Node("block", hash_rule_name("block")),
            owner("dummy")
        {}

        Block(std::string str, U64 hash, std::string owner_name, int _target_num_qubits) : 
            Node(str, hash),
            owner(owner_name), 
            target_num_qubits(_target_num_qubits) {}

        inline bool owned_by(std::string other){return other == owner;}

        inline std::string get_owner(){return owner;}

        void set_can_apply_subroutines(bool flag){
            can_apply_subroutines = flag;
        }

        bool get_can_apply_subroutines() const {
            return can_apply_subroutines;
        }

        void set_target_num_qubits_int(){
            target_num_qubits_int = random_int(Common::MAX_QUBITS, Common::MIN_QUBITS);
        }

        inline size_t num_external_qubits() const {
            return external_qubits.size();
        }

        inline size_t num__internal_qubits() const {
            return internal_qubits.size();
        }

        /* 
        Counts the number of qubit defiitions, depending on if counting internal or external definitions
        (for branching purposes)
         */
        inline size_t num_qubit_definitions(bool external) const {
            if (external) {
                return std::count_if(qubit_defs.begin(), qubit_defs.end(),
                [](const Qubit_definition::Qubit_definition& def) {
                    return def.get_ext_in();
                });
            } else {
                return std::count_if(qubit_defs.begin(), qubit_defs.end(),
                [](const Qubit_definition::Qubit_definition& def) {
                    return !def.get_ext_in();
                });
            }
        }

        void qubit_flag_reset(){
            for(Qubit::Qubit& qb : external_qubits){
                qb.reset();
            }
        }

        inline std::shared_ptr<Qubit_definition::Qubit_definition> get_next_qubit_def(){
            if((size_t)qubit_def_pointer < qubit_defs.size()){
                return std::make_shared<Qubit_definition::Qubit_definition>(qubit_defs[qubit_def_pointer++]);
            } else {
                return std::make_shared<Qubit_definition::Qubit_definition>(dummy_def);
            }
        }

        std::shared_ptr<Qubit::Qubit> get_random_qubit(std::optional<std::vector<int>> best_entanglement);
        
        size_t make_register_qubit_definition(int max_size, bool external = true);

        size_t make_singular_qubit_definition(bool external = true);

        void make_qubit_definitions(bool external = true);

        Qubit::Qubit* get_qubit_at(size_t index);

    private:
        std::string owner;
        int target_num_qubits = Common::MIN_QUBITS;
        int target_num_qubits_int = Common::MIN_QUBITS;
        
        bool can_apply_subroutines = false;

        std::vector<Qubit::Qubit> external_qubits;
        std::vector<Qubit::Qubit> internal_qubits;
        std::vector<Qubit_definition::Qubit_definition> qubit_defs;

        Qubit::Qubit dummy_qubit;
        Qubit_definition::Qubit_definition dummy_def;

        int qubit_def_pointer = 0;

        int register_qubit_def_count = 0;
        int singular_qubit_def_count = 0;
};


#endif
