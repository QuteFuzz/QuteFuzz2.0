#ifndef BLOCK_H
#define BLOCK_H

#include <node.h>
#include <resource_definition.h>
#include <resource.h>
#include <collection.h>

class Block : public Node {

    public:

        Block() : 
            Node("block", hash_rule_name("block")),
            owner("dummy")
        {}

        Block(std::string str, U64 hash, std::string owner_name, int _target_num_qubits_external, int _target_num_qubits_internal, 
            int _target_num_bits_external, int _target_num_bits_internal) : 
            Node(str, hash),
            owner(owner_name), 
            target_num_qubits_external(_target_num_qubits_external),
            target_num_qubits_internal(_target_num_qubits_internal),
            target_num_bits_external(_target_num_bits_external),
            target_num_bits_internal(_target_num_bits_internal) {}

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

        inline size_t num_owned_qubits() const {
            return qubits.get_num_owned();
        }

        inline size_t num_internal_qubit_defs() const {
            return qubit_defs.get_num_internal();
        }

        inline size_t num_owned_qubit_defs() const {
            return qubit_defs.get_num_owned();
        }

        inline size_t num_external_qubit_defs() const {
            return qubit_defs.get_num_external();
        }

        inline size_t total_num_bits() const {
            return bits.get_total();
        }

        inline size_t num_external_bits() const {
            return bits.get_num_external();
        }

        inline size_t num_internal_bits() const {
            return bits.get_num_internal();
        }

        inline size_t num_external_bit_defs() const {
            return bit_defs.get_num_external();
        }

        inline size_t num_internal_bit_defs() const {
            return bit_defs.get_num_internal();
        }

        void qubit_flag_reset(){
            qubits.reset();
        }

        void bit_flag_reset(){
            bits.reset();
        }

        void qubit_def_pointer_reset(){
            qubit_def_pointer = 0;
        }

        void bit_def_pointer_reset(){
            bit_def_pointer = 0;
        }

        inline Resource::Resource* qubit_at(size_t index){
            if(index < qubits.get_total()){
                return qubits.at(index);
            } else {
                return &dummy_qubit;
            }
        }

        inline Resource::Resource* bit_at(size_t index){
            if(index < bits.get_total()){
                return bits.at(index);
            } else {
                return &dummy_bit;
            }
        }

        Collection<Resource::Qubit> get_qubits(){
            return qubits;
        }

        Collection<Resource::Bit> get_bits(){
            return bits;
        }

        Resource::Qubit* get_random_qubit(U8 scope_filter = ALL_SCOPES);
        
        Resource::Bit* get_random_bit(U8 scope_filter = ALL_SCOPES);

        std::shared_ptr<Qubit_definition> get_next_qubit_def(U8 scope_filter = ALL_SCOPES);

        std::shared_ptr<Bit_definition> get_next_bit_def(U8 scope_filter = ALL_SCOPES);

        size_t make_register_resource_definition(int max_size, U8 scope, Resource::Classification classification, size_t& total_definitions);

        size_t make_singular_resource_definition(U8 scope, Resource::Classification classification, size_t& total_definitions);

        size_t make_resource_definitions(U8 scope, Resource::Classification classification);

    private:
        std::string owner;
        int target_num_qubits_external = Common::MIN_QUBITS;
        int target_num_qubits_internal = 0;
        int target_num_bits_external = Common::MIN_BITS;
        int target_num_bits_internal = 0;
        
        bool can_apply_subroutines = true;

        Collection<Resource::Qubit> qubits;
        Collection<Qubit_definition> qubit_defs;

        Collection<Resource::Bit> bits;
        Collection<Bit_definition> bit_defs;

        size_t qubit_def_pointer = 0;
        size_t bit_def_pointer = 0;

        Resource::Qubit dummy_qubit;
        Resource::Bit dummy_bit;

        Qubit_definition dummy_qubit_def;
        Bit_definition dummy_bit_def;

};


#endif
