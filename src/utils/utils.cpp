#include <utils.h>

namespace Common {
    bool plot = false;
    bool verbose = false;
    bool render_qigs = false;
}

void lower(std::string& str){
    std::transform(str.begin(), str.end(), str.begin(),
        [](char c){return std::tolower(c);}
    );
}

/// @brief Lowercase input for consistency, and ease. Uses FNV hash. Tried to find a good hash so as to avoid collisions
/// @param rule_name 
/// @return 
U64 hash_rule_name(std::string rule_name) {
    U64 hash = 14695981039346656037ULL;  // FNV offset basis
    const U64 prime = 1099511628211ULL;  // FNV prime

    lower(rule_name);

    for (const char& c : rule_name) {
        hash ^= static_cast<uint8_t>(c);  
        hash *= prime;                     // multiply by a prime
    }

    return hash;
}

std::mt19937& seed(){
    static std::mt19937 random_gen{
        std::random_device{}()
    };
    return random_gen;
}

/// @brief Random integer within some range
/// @param max value inclusive
/// @param min value inclusive
/// @return
int random_int(int max, int min){
    if(min < max){
        std::uniform_int_distribution<int> int_dist(min, max);
        return int_dist(seed());

    } else {
        return min;
    }
}

/// @brief Random float within some range
/// @param max value inclusive
/// @param min value inclusive
/// @return 
float random_float(float max, float min){
    if(min < max){
        std::uniform_real_distribution<float> float_dist(min, max);
        return float_dist(seed());

    } else {
        return min;
    }
}


std::optional<int> safe_stoi(const std::string& str) {
    try {
        int ret = (str == "") ? 1 : std::stoi(str);
        return std::optional<int>(ret);
    } catch (const std::invalid_argument& e) {
        ERROR(e.what());
        return std::nullopt;
    }
}

/// @brief Find all possible combinations that can be chosen from numbers in [0, n-1]
/// Knew the solution had something to do with counting in binary, but I didn't come up with this algorithm myself
/// https://stackoverflow.com/questions/12991758/creating-all-possible-k-combinations-of-n-items-in-c
/// @param n 
/// @param r 
/// @return 
std::vector<std::vector<int>> n_choose_r(const int n, const int r){
    if((n >= r) && (r >= Common::MIN_N_QUBITS_IN_ENTANGLEMENT)){

        std::vector<std::vector<int>> res;

        std::string bitmask(r, 1);
        bitmask.resize(n, 0);

        do{
            std::vector<int> comb;

            for(int i = 0; i < n; i++){
                if (bitmask[i]) comb.push_back(i);
            }

            res.push_back(comb);

        } while(std::prev_permutation(bitmask.begin(), bitmask.end()));

        return res;

    } else {
        throw std::runtime_error(ANNOT("n must be less than r, and r must be >= " + std::to_string(Common::MIN_N_QUBITS_IN_ENTANGLEMENT)));
    }
}


int vector_sum(std::vector<int> in){
    int res = 0;

    for(size_t i = 0; i < in.size(); i++){
        res += in[i];
    }

    return res;
}

int vector_max(std::vector<int> in){
    int max = 0;

    for(size_t i = 0; i < in.size(); i++){
        max = std::max(max, in[i]);
    }

    return max;
}


void pipe_to_command(std::string command, std::string write){
    FILE* pipe = popen(command.c_str(), "w");

    if(!pipe){
        throw std::runtime_error(ANNOT("Failed to open pipe to command " + command));
    }

    fwrite(write.c_str(), sizeof(char), write.size(), pipe);
    
    if(pclose(pipe)){
        throw std::runtime_error(ANNOT("Command " + command + " failed"));
    }

    if(Common::verbose){
        INFO("Run command " + command);
        INFO("Piped " + write + " to command");
    }
}

std::string pipe_from_command(std::string command){
    FILE* pipe = popen(command.c_str(), "r");

    if(!pipe){
        ERROR("Failed to open pipe to command " + command);
    }

    std::array<char, 1024> buffer;
    std::string result = "";

    while(fgets(buffer.data(), buffer.size(), pipe) != nullptr){
        result += buffer.data();
    }

    if(pclose(pipe)){
        ERROR("Command " + command + " failed");
    }

    if(Common::verbose){
        INFO("Run command " + command);
    }

    return result;
}

std::string escape(const std::string& str){
    std::ostringstream oss;
    oss << std::quoted(str);
    return oss.str();
}