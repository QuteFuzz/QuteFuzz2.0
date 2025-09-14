#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include <string>
#include <iostream>
#include <variant>
#include <regex>
#include <random>
#include <set>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <optional>
#include <array>
#include <iomanip>

#define WILDCARD_MAX 5

#define UNUSED(x) (void)(x)

// colours
#define RED(x) (std::string("\033[31m") + x + std::string("\033[0m"))
#define YELLOW(x) (std::string("\033[33m") + x + std::string("\033[0m"))
#define GREEN(x) (std::string("\033[32m") + x + std::string("\033[0m"))

// location annotation
#define ANNOT(x) (std::string("at ") + __FILE__ + "," + std::to_string(__LINE__) + ": " + (x))

// logging
#define ERROR(x) std::cerr << (std::string("[ERROR] ") + RED(ANNOT(x))) << std::endl
#define WARNING(x) std::cout << (std::string("[WARNING] ") + YELLOW(ANNOT(x))) << std::endl
#define INFO(x) std::cout << (std::string("[INFO] ") + GREEN(x)) << std::endl

// flag status
#define FLAG_STATUS(x) (x ? YELLOW("enabled") : YELLOW("disabled"))

#define NO_SCOPE 0
#define EXTERNAL_SCOPE (1UL << 0)
#define INTERNAL_SCOPE (1UL << 1)
#define OWNED_SCOPE (1UL << 2)
#define ALL_SCOPES (EXTERNAL_SCOPE | INTERNAL_SCOPE | OWNED_SCOPE)

#define STR_SCOPE(scope) (" scope flag (OWNED  INTERNAL  EXTERNAL): " + std::bitset<8>(scope).to_string())

using U64 = uint64_t;

using U8 = uint8_t;

namespace fs = std::filesystem;

U64 hash_rule_name(std::string rule_name);

void lower(std::string& str);

std::mt19937& seed();

int random_int(int max, int min = 0);

float random_float(float max, float min = 0.0);

std::optional<int> safe_stoi(const std::string& str);

std::vector<std::vector<int>> n_choose_r(const int n, const int r);

int vector_sum(std::vector<int> in);

int vector_max(std::vector<int> in);

void pipe_to_command(std::string command, std::string write);

std::string pipe_from_command(std::string command);

std::string escape(const std::string& str);

std::string random_hex_colour();

template<typename T>
std::vector<T> multiply_vector(std::vector<T> vec, int mult){
    std::vector<T> multiplied_vec;
    
    multiplied_vec.reserve(vec.size() * mult);

    for(int i = 0; i < mult; ++i){
        multiplied_vec.insert(multiplied_vec.end(), vec.begin(), vec.end());
    }

    return multiplied_vec;
}

template<typename T>
std::vector<T> append_vectors(std::vector<T> vec1, std::vector<T> vec2){
    std::vector<T> result = vec1;

    result.insert(result.end(), vec2.begin(), vec2.end());

    return result;
}

namespace Common {
    /*
        names 
    */
    constexpr char TOP_LEVEL_CIRCUIT_NAME[] = "main_circuit";
    constexpr char OUTPUTS_FOLDER_NAME[] = "outputs";
    constexpr char META_GRAMMAR_NAME[] = "meta-grammar";

    /*
        ast parameters
    */
    constexpr int MIN_N_QUBITS_IN_ENTANGLEMENT = 2;
    constexpr int MIN_QUBITS = 3;
    constexpr int MIN_BITS = 1;
    constexpr int MAX_QUBITS = 4; // std::max(MIN_QUBITS + 1, (int)(0.5 * WILDCARD_MAX));
    constexpr int MAX_BITS = 2; // std::max(MIN_BITS + 1, (int)(0.5 * WILDCARD_MAX));
    constexpr int MAX_SUBROUTINES = 2; //  (int)(0.5 * WILDCARD_MAX);
    constexpr int NESTED_MAX_DEPTH = 2;
    constexpr int SWARM_TESTING_GATESET_SIZE = 6;

    /*
        flags 
    */
    extern bool plot;
    extern bool verbose;
    extern bool render_dags;
    extern bool run_genetic;
    extern bool swarm_testing;
    
    /*
        ast node types
    */
    enum Rule_hash : U64 {
        /*
            gates
            - all rules denoting gates
        */
        // SINGLE QUBIT GATES
        h = 12638197096160295895ULL,
        x = 12638214688346347271ULL,
        y = 12638213588834719060ULL,
        z = 12638216887369603693ULL,
        rz = 638471042196278621ULL,
        rx = 638468843173022199ULL,
        ry = 638467743661393988ULL,
        u1 = 631765120777144307ULL,
        s = 12638206991764949794ULL,
        sdg = 9383135583349391919ULL,
        t = 12638201494206808739ULL,
        tdg = 6262443406867765286ULL,
        v = 12638203693230065161ULL,
        vdg = 7571990343028037872ULL,
        phasedxpowgate = 11832025932794728373ULL,
        project_z = 2608459580584910443ULL,
        measure_and_reset = 15416528854723921477ULL,
        Measure = 13586824709325088415ULL,

        // TWO QUBIT GATES
        cx = 622136697450792830ULL,
        cy = 622137796962421041ULL,
        cz = 622134498427536408ULL,
        ccx = 17716034042247149281ULL,
        u2 = 631766220288772518ULL,
        cnot = 352634892887392207ULL,
        ch = 622154289636844206ULL,
        crz = 17699985570524998350ULL,
    
        // THREE QUBIT GATES    
        u3 = 631767319800400729ULL,
        cswap = 9091793091986225957ULL,
        toffoli = 15991066036535617246ULL,

        // OTHER GATES
        u = 12638200394695180528ULL,
        barrier = 11527731894837406848ULL,

        /*
            rules that need special attention during AST building
        */
        subroutine_defs = 16338711102002774808ULL,

        circuit_def = 17654104105659481736ULL,

        body = 14793735007222968981ULL,

        qubit_defs = 10682880043223733083ULL,
        bit_defs = 882498004770982501ULL,
        qubit_def = 11301569403047543754ULL,
        register_qubit_def = 16393191790828246050ULL,
        singular_qubit_def = 3663840257195318050ULL,
        bit_def = 11428494716284192756ULL,
        register_bit_def = 7591478845252401516ULL,
        singular_bit_def = 14132065875484436588ULL,

        circuit_name = 5389401364268778602ULL,
        float_list = 12337679196430086138ULL,
        float_literal = 6014115549703600901ULL,
        main_circuit_name = 15359974437464362266ULL,

        qubit_def_name = 9637820166840754028ULL,
        bit_def_name = 10693244643305520522ULL,
        qreg_size = 11502232252882731618ULL,
        creg_size = 4440641367620343308ULL,
        qubit = 9613145697666387222ULL,
        bit = 23676991090714736ULL,
        qubit_op = 7363837753828900628ULL,
        gate_op = 17845938762380861480ULL,
        subroutine_op = 4275855153236832595ULL,

        gate_name = 4107851538286704628ULL,
        qubit_list = 18380990572907722739ULL,
        bit_list = 3372974018932553881ULL,
        qubit_def_list = 12288968177510665943ULL, 
        qubit_def_size = 16541258826307231238ULL, 
        bit_def_list = 8034270308867516509ULL,
        bit_def_size = 2428654514965833632ULL,

        singular_qubit = 358282167276964286ULL,
        register_qubit = 1224902788537856702ULL,
        singular_bit = 9032933660288388984ULL,
        register_bit = 17133561988447867000ULL,

        qubit_name = 8757953683662904688ULL,
        bit_name = 6055046893651473182ULL,
        qubit_index = 6830264791288854081ULL,
        bit_index = 8231177906794104931ULL,

        subroutine = 7419198593375467891ULL,
        circuit_id = 12523072865437646660ULL,
        total_num_qubits = 11347479691534777333ULL,

        compound_stmt = 4421131487532525991ULL,
        compound_stmts = 17100464358870324028ULL,
        
        indent = 8881161079555216635ULL,
        dedent = 2224769550356995471ULL,

        if_stmt = 5300980200188049869ULL,
        subroutine_op_args = 4098213824854424185ULL,
        gate_op_args = 5968085974550298764ULL,
        subroutine_op_arg = 11699520117738358096ULL,
        else_stmt = 2582766405432659795ULL,
        elif_stmt = 9453565397799917274ULL,
        disjunction = 13554539731759707019ULL,
        conjunction = 15291463859833294983ULL,
        inversion = 6572250267686125186ULL,
        expression = 17035010316065342411ULL,
        compare_op_bitwise_or_pair = 11179282617234990551ULL,

        number = 9237349086447201248ULL,
    };
}

#endif

