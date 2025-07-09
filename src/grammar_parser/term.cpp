#include <term.h>
#include <rule.h>

void Term::set(std::shared_ptr<Rule> term){
    value = term;
    _name = term->get_name();
    hashed_name = term->get_hash();
}

void Term::set(std::string syntax){
    value = syntax;
    _name = syntax;
}

std::shared_ptr<Rule> Term::get_rule() const {
    if(is_pointer()){return std::get<std::shared_ptr<Rule>>(value);}
    else {
        throw std::runtime_error("get_term called on syntax!");
    }
}

std::string Term::get_syntax() const {
    if(is_syntax()){return std::get<std::string>(value);}
    else {
        throw std::runtime_error("get_syntax called on pointer!");
    }
}

std::string Term::get_string() const {
    return _name;
}

U64 Term::get_hash() const {
    return hashed_name;
}

bool Term::is_syntax() const {
    return std::holds_alternative<std::string>(value);
}

bool Term::is_pointer() const {
    return std::holds_alternative<std::shared_ptr<Rule>>(value);
}

std::ostream& operator<<(std::ostream& stream, Term term){
    if(term.is_syntax()){
        if(term._name == "\n"){
            stream << "\"\\n\"";

        } else {
            stream << "\"" << term._name << "\"";
    
        }
        
    } else {
        stream << term._name;
    }

    return stream;
}

void Term::set_nesting_depth(unsigned int nd){nesting_depth = nd;}

unsigned int Term::get_nesting_depth() const {return nesting_depth;}

bool Term::operator==(const Term& other) const {
    return hashed_name == other.get_hash();
}

bool Term::operator==(const U64& hash) const {
    return hashed_name == hash;
}
