#ifndef RULE_H
#define RULE_H

#include <branch.h>

class Node;

class Rule {

    public:
        Rule(){}
        
        Rule(const std::string& _name, const U8& _scope) : name(_name), scope(_scope) {
            hash = hash_rule_name(name);
        }
        
        Rule(const std::vector<Branch>& _branches) : branches(_branches) {}

        ~Rule(){}

        std::string get_name() const {return name;}

        U64 get_hash() const {return hash;}

        U8 get_scope() const {return scope;}

        bool get_recursive_flag() const {return recursive;}
                
        std::vector<Branch> get_branches(){return branches;}

        void add(const Branch& b);

        inline size_t size(){return branches.size();}

        inline bool is_empty() const {return branches.empty();}

        inline void clear(){branches.clear();}

        Branch pick_branch(std::shared_ptr<Node> parent);

        bool operator==(const Rule& other) const { return (name == other.get_name()) && (scope == other.get_scope()); }

        friend std::ostream& operator<<(std::ostream& stream, const Rule& rule){
            stream << rule.name << " = ";
            
            for(size_t i = 0; i < rule.branches.size(); i++){
                stream << rule.branches[i];
                if(i < rule.branches.size() - 1) stream << " | ";
            }

            stream << " ; " << STR_SCOPE(rule.scope) << std::endl;

            return stream;
        }

    private:
        std::string name;
        U8 scope = NO_SCOPE;

        U64 hash = 0ULL;
        bool recursive = false;
    
        std::vector<Branch> branches;

};

#endif