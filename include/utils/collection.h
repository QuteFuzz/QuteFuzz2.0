#ifndef COLLECTION_H
#define COLLECTION_H

namespace Resource {
    class Qubit;
    class Bit;
}

class Qubit_definition;
class Bit_definition;

template<typename T>
concept Allowed_Type = 
    std::is_same_v<T, Resource::Qubit> ||
    std::is_same_v<T, Resource::Bit> || 
    std::is_same_v<T, Qubit_definition> || 
    std::is_same_v<T, Bit_definition>;

template<Allowed_Type T>
struct Collection {

    public:

        Collection(){}

        void add(const T& elem){
            if(elem.is_external()){
                num_external += 1;
            } else {
                num_internal += 1;
            }

            if(elem.is_owned()){
                num_owned += 1;
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

        size_t get_num_owned() const {
            return num_owned;
        }

        size_t get_total() const {
            return num_internal + num_external;
        }

        void reset(){
            if constexpr (std::is_same_v<T, Resource::Qubit> || std::is_same_v<T, Resource::Bit>){
                for(T& elem : coll){
                    elem.reset();
                }
            }
        }

        std::vector<T>::iterator begin(){
            return coll.begin();
        }

        std::vector<T>::iterator end(){
            return coll.end();
        }

        std::vector<T>::const_iterator begin() const {return coll.begin();}

        std::vector<T>::const_iterator end() const {return coll.end();}

    private:
    
        std::vector<T> coll = {};
        size_t num_internal = 0;
        size_t num_external = 0;
        size_t num_owned = 0;
};


#endif
