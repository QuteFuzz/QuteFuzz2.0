#ifndef COLLECTION_H
#define COLLECTION_H

#include <qubit.h>

template<typename T>
struct Collection {

    public:

        Collection(){}

        void add(const T& elem){
            if(elem.is_external()){
                num_external += 1;
            } else {
                num_internal += 1;
            }

            coll.push_back(elem);
        }

        T* at(size_t index) {
            if(index < get_total()){
                return &coll.at(index); 

            } else {
                return nullptr;
            
            }
        }

        size_t get_num_internal() const {
            return num_internal;
        }

        size_t get_num_external() const {
            return num_external;
        }

        size_t get_total() const {
            return coll.size();
        }

        void reset(){
            if constexpr (std::is_same_v<T, Qubit::Qubit>){
                for(Qubit::Qubit& qb : coll){
                    qb.reset();
                }
            }
        }

    private:
    
        std::vector<T> coll = {};
        size_t num_internal = 0;
        size_t num_external = 0;

};


#endif
