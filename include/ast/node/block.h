#ifndef BLOCK_H
#define BLOCK_H

#include <node.h>
#include <resource_definition.h>
#include <resource.h>
#include <collection.h>

/*
    Blocks contain external and internal qubits, external and internal bits, which are set targets that must be satisfied
    It is not a guarantee that any block will have both internal and external qubits, so the targets are set such that any block 
    will have external qubits = (MIN_QUBITS, MAX_QUBITS) and internal qubits = (MIN_QUBITS, MAX_QUBITS) separately, 
    instead of external qubits + internal qubits = (MIN_QUBITS, MAX_QUBITS)

    See example below, if using  external qubits + internal qubits = (MIN_QUBITS, MAX_QUBITS)

    =======================================
                BLOCK INFO               
    =======================================
    Owner: main_circuit
    Target num qubits 
    EXTERNAL: 3
    INTERNAL: 1
    Target num bits 
    EXTERNAL: 1
    INTERNAL: 1

    Qubit definitions 
    Qubit defs may not match target if block is built to match DAG
    name: qreg0 size: 1 Scope: internal
    =======================================
    @guppy.comptime
    def main_circuit() -> None:
            qreg0 = array(qubit() for _ in range(1))
            if 0 <= 0  and 0 > 0   or 0 <= 0  and 0 == 0   :
                    if 0 <= 0  and 0 == 0   or 0 >= 0  and 0 <= 0   :
                            project_z(qreg0[0])
                            cy(qreg0[0], 

    the block set a target for internal qubits of 1, and external of 3. But since this is guppy, only internal definitions are created, and therefore this stalls in 
    an infinite loop while picking a random qubit
*/

class Block : public Node {

    public:

        Block() : 
            Node("block", Common::block),
            owner("dummy")
        {}

        /// @brief Generating a random block from scratch
        /// @param str 
        /// @param hash 
        /// @param owner_name 
        Block(std::string str, U64 hash, std::string owner_name) :
            Node(str, hash),
            owner(owner_name), 
            target_num_qubits_external(random_int(Common::MAX_QUBITS, Common::MIN_QUBITS)),
            target_num_qubits_internal(random_int(Common::MAX_QUBITS, Common::MIN_QUBITS)),
            target_num_bits_external(random_int(Common::MAX_BITS, Common::MIN_BITS)),
            target_num_bits_internal(random_int(Common::MAX_BITS, Common::MIN_BITS)) 
        {}

        /// @brief Generating a block with a specific number of external qubits (generating from DAG)
        /// @param str 
        /// @param hash 
        /// @param owner_name 
        /// @param num_external_qubits 
        Block(std::string str, U64 hash, std::string owner_name, unsigned int num_external_qubits) :
            Node(str, hash),
            owner(owner_name), 
            target_num_qubits_external(num_external_qubits),
            target_num_qubits_internal(random_int(Common::MAX_QUBITS, Common::MIN_QUBITS)),
            target_num_bits_external(random_int(Common::MAX_BITS, Common::MIN_BITS)),
            target_num_bits_internal(random_int(Common::MAX_BITS, Common::MIN_BITS)) 
        {}

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

        inline size_t num_owned_bits() const {
            return bits.get_num_owned();
        }

        inline size_t num_external_bit_defs() const {
            return bit_defs.get_num_external();
        }

        inline size_t num_internal_bit_defs() const {
            return bit_defs.get_num_internal();
        }

        inline size_t num_owned_bit_defs() const {
            return bit_defs.get_num_owned();
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

        inline Collection<Resource::Qubit> get_qubits(){
            return qubits;
        }

        inline Collection<Resource::Bit> get_bits(){
            return bits;
        }

        Resource::Qubit* get_random_qubit(U8 scope_filter = ALL_SCOPES);
        
        Resource::Bit* get_random_bit(U8 scope_filter = ALL_SCOPES);

        std::shared_ptr<Qubit_definition> get_next_qubit_def(U8 scope_filter = ALL_SCOPES);

        std::shared_ptr<Bit_definition> get_next_bit_def(U8 scope_filter = ALL_SCOPES);

        unsigned int make_register_resource_definition(unsigned int max_size, U8 scope, Resource::Classification classification, unsigned int& total_definitions);

        unsigned int make_singular_resource_definition(U8 scope, Resource::Classification classification, unsigned int& total_definitions);

        unsigned int make_resource_definitions(U8 scope, Resource::Classification classification);

        unsigned int make_resource_definitions(U8 scope, const Collection<Resource::Qubit>& _qubits);

        unsigned int qubit_to_qubit_def(const U8& scope, const Resource::Qubit& qubit);

        void print_info() const;

    private:
        std::string owner;

        unsigned int target_num_qubits_external = Common::MIN_QUBITS;
        unsigned int target_num_qubits_internal = 0;
        unsigned int target_num_bits_external = Common::MIN_BITS;
        unsigned int target_num_bits_internal = 0;
        
        bool can_apply_subroutines = true;

        Collection<Resource::Qubit> qubits;
        Collection<Qubit_definition> qubit_defs;

        Collection<Resource::Bit> bits;
        Collection<Bit_definition> bit_defs;

        unsigned int qubit_def_pointer = 0;
        unsigned int bit_def_pointer = 0;

        Resource::Qubit dummy_qubit;
        Resource::Bit dummy_bit;

        Qubit_definition dummy_qubit_def;
        Bit_definition dummy_bit_def;

};


#endif
