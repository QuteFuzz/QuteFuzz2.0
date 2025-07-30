#ifndef BLOCK_H
#define BLOCK_H

#include <node.h>
#include <qubit_definition.h>
#include <qubit.h>
#include <collection.h>

class Block : public Node {

    public:

        Block() : 
            Node("block", hash_rule_name("block")),
            owner("dummy")
        {}

        Block(std::string str, U64 hash, std::string owner_name, int _target_num_qubits_external, int _target_num_qubits_internal) : 
            Node(str, hash),
            owner(owner_name), 
            target_num_qubits_external(_target_num_qubits_external),
            target_num_qubits_internal(_target_num_qubits_internal) {}

        inline bool owned_by(std::string other){return other == owner;}

        inline std::string get_owner(){return owner;}

        void set_can_apply_subroutines(bool flag){
            can_apply_subroutines = flag;
        }

        bool get_can_apply_subroutines() const {
            return can_apply_subroutines;
        }

        inline size_t total_num_qubits() const {
            return qubits.get_total();
        }

        inline size_t num_external_qubits() const {
            return qubits.get_num_external();
        }

        inline size_t num_internal_qubits() const {
            return qubits.get_num_internal();
        }

        inline size_t num_internal_qubit_defs() const {
            return qubit_defs.get_num_internal();
        }

        inline size_t num_external_qubit_defs() const {
            return qubit_defs.get_num_external();
        }

        void qubit_flag_reset(){
            qubits.reset();
        }

        void qubit_def_pointer_reset(){
            qubit_def_pointer = 0;
        }

        inline Qubit::Qubit* qubit_at(size_t index){
            if(index < qubits.get_total()){
                return qubits.at(index);
            } else {
                return &dummy_qubit;
            }
        }

        std::shared_ptr<Qubit_definition::Qubit_definition> get_next_qubit_def();

        std::shared_ptr<Qubit_definition::Qubit_definition> get_next_owned_qubit_def();

        std::shared_ptr<Qubit_definition::Qubit_definition> get_next_external_qubit_def();

        std::shared_ptr<Qubit::Qubit> get_random_qubit(std::optional<std::vector<int>> best_entanglement, bool internal_only = false);
        
        size_t make_register_qubit_definition(int max_size, bool external);

        size_t make_singular_qubit_definition(bool external);

        size_t make_qubit_definitions(bool external);

    private:
        std::string owner;
        int target_num_qubits_external = Common::MIN_QUBITS;
        int target_num_qubits_internal = 0;
        
        bool can_apply_subroutines = true;

        Collection<Qubit::Qubit> qubits;
        Collection<Qubit_definition::Qubit_definition> qubit_defs;

        size_t qubit_def_pointer = 0;
        Qubit::Qubit dummy_qubit;
        Qubit_definition::Qubit_definition dummy_def;

};


#endif
