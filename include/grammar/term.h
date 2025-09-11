#ifndef TERM_H
#define TERM_H

#include <utils.h>

class Rule;

class Term {
    public:
        Term(){}
        Term(const std::string& name, unsigned int nd, unsigned int id = 0): _name(name), nesting_depth(nd), indent_depth(id) {}
        ~Term() = default;

        void set(std::shared_ptr<Rule> term);

        void set(std::string syntax);

        std::shared_ptr<Rule> get_rule() const;

        std::string get_syntax() const;

        std::string get_string() const;

        U64 get_hash() const;

        U8 get_scope() const;

        bool is_syntax() const;

        bool is_pointer() const;

        friend std::ostream& operator<<(std::ostream& stream, Term term);

        void set_nesting_depth(unsigned int nd);

        unsigned int get_nesting_depth() const;

        bool operator==(const Term& other) const;

        bool operator==(const U64& hash) const;

    private:
        std::variant<std::shared_ptr<Rule>, std::string> value;

        std::string _name;
        U64 hashed_name = 0ULL;
        unsigned int nesting_depth = 0;
        unsigned int indent_depth = 0;
};

#endif

