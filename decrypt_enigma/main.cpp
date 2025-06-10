// A terminal utility to decrypt Enigma-encrypted text. 

// g++ main.cpp cli_interface.hpp denigma.hpp denigma_fitness.cpp denigma_force.cpp denigma_key.cpp encrypt_enigma/enigma.hpp encrypt_enigma/enigma_impl.hpp encrypt_enigma/enigma_machine.cpp encrypt_enigma/enigma_rotors.cpp encrypt_enigma/enigma_key.cpp -o decrypt_enigma

// x86_64-w64-mingw32-g++-posix -std=c++20 -static main.cpp cli_interface.hpp denigma.hpp denigma_fitness.cpp denigma_force.cpp denigma_key.cpp encrypt_enigma/enigma.hpp encrypt_enigma/enigma_impl.hpp encrypt_enigma/enigma_machine.cpp encrypt_enigma/enigma_rotors.cpp encrypt_enigma/enigma_key.cpp -o decrypt_enigma.exe

#include"denigma.hpp"
#include"cli_interface.hpp"

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<map>
#include<algorithm>

using namespace std;
using namespace Denigma;


// input handling function declarations 
string sanitize_ciphertext(const string& str); 
vector<int> sanitize_possible_rotors(const vector<string>& vec);
vector<int> get_fitness_functions(const vector<string>& vec);

void print_help();

int main(int argc, char* argv[]) { // + 1, Remove program name.
    const vector<string> args = {argv + 1, argv + argc}; 
    
    // If the user only asked for the help menu, only print that.
    if (args[0] == "-h" || args[0] == "--help") { 
        print_help();
        return 0;
    }
    
    // Process the CLI input into its parts. 
    vector<string> flags = {"--ciphertext", "--key", "--possible_rotors", "--amount", "--fitness", "--known_plaintext"};
    map<string,vector<string>> inputs = cli_interface::get_block_arguments(args, flags);
    
    // Get the ciphertext from the specified filepath. 
    if (!inputs.count("--ciphertext")) { cout << "Please specify a filepath to the ciphertext.\n"; return 1; }
    string ct = "";
    ifstream ifs(inputs["--ciphertext"][0]);
    if (ifs) {
        char c = '\0';
        while (ifs.get(c)) { ct += c; }
        ifs.close(); 
    } else {
        cerr << "Invalid filepath to ciphertext.\n";
        return 1;
    }
    const string ciphertext = sanitize_ciphertext(ct);
    
    // Get the known plaintext, if a filepath was specified. 
    string kp = "";
    if (inputs.count("--known_plaintext")) {
        ifs.open(inputs["--known_plaintext"][0]);
        if (ifs) {
            char c = '\0';
            while (ifs.get(c)) { kp += c; }
            ifs.close(); 
        } else {
            cerr << "Invalid filepath to known plaintext.\n";
            return 1;
        }
    }
    const string known_plaintext = kp;
    
    // Create a function object for each fitness function and put them together
    IoC ioc;
    Letters letters;
    Bigrams bigrams;
    Trigrams trigrams;
    Quadgrams quadgrams;
    Known known(known_plaintext);
    const vector<Fitness_Function*> fitness_functions = {&ioc, &letters, &bigrams, &trigrams, &quadgrams, &known};
    
    // Handle the rest of the inputted data, or use defaults. 
    const Enigma::Key enigma_key = (inputs.count("--key") ? Enigma::Key(inputs["--key"]) : Enigma::Key());
    const vector<int> possible_rotors = (inputs.count("--possible_rotors") ? sanitize_possible_rotors(inputs["--possible_rotors"]) : vector<int>{0,1,2,3,4}); 
    const int amount = (inputs.count("--amount") ? stoi(inputs["--amount"][0]) : 1);
    const vector<int> ff = (inputs.count("--fitness") ? get_fitness_functions(inputs["--fitness"]) : vector<int>{0,0,0}); 
    
    // If a part of the decryptor was specified, run that. Otherwise default to
    // the whole process.     
    if (args[0] == "--rotors" || args[0] == "--names") {
        const vector<Scored_Key> sckeys = force_rotor_name(ciphertext, possible_rotors, amount, *fitness_functions[ff[0]], enigma_key.plugs);
        for (const auto& sckey : sckeys)
            cout << string(sckey) << "\n";
            
    } else if (args[0] == "--pos" || args[0] == "--positions") {
        const vector<Scored_Rotors> scrtrs = force_rotor_pos(ciphertext, {{ enigma_key.rotors[0][0], enigma_key.rotors[1][0], enigma_key.rotors[2][0] }}, amount, *fitness_functions[ff[0]], enigma_key.plugs);
        for (const auto& scrtr : scrtrs)
            cout << string(Scored_Key{scrtr, enigma_key.plugs}) << "\n";
            
    } else if (args[0] == "--rings" || args[0] == "--notches") {
        const vector<Scored_Key> sckeys = force_rotor_notch(ciphertext, enigma_key, amount, *fitness_functions[ff[1]]);
        for (const auto& sckey : sckeys)
            cout << string(sckey) << "\n";
            
    } else if (args[0] == "--plugs" || args[0] == "--plugboard") {
        const vector<Scored_Plugboard> scplgs = force_plugboard(ciphertext, enigma_key, *fitness_functions[ff[2]]);
        for (const auto& scplg : scplgs)
            cout << string(Scored_Key{enigma_key.rotors, scplg}) << "\n";
            
    } else if (args[0] == "--encrypt" || args[0] == "--enigma") {
        cout << Enigma::Machine(enigma_key).fast_encrypt(ciphertext) << "\n";
            
    } else {
        cout << string(force_ciphertext(ciphertext, possible_rotors, *fitness_functions[ff[1]], *fitness_functions[ff[1]], *fitness_functions[ff[2]])) << "\n";
    }
    
    return 0;
}


// Turn the input string into pure uppercase letters. 
string sanitize_ciphertext(const string& str) {
    string result = ""; 
    for (const char& c : str) 
        if (isalpha(c)) 
            result += toupper(c); 
    return result;
}

// Read Roman Numerals or ints, and return a vector containing ints 0 to 7.
vector<int> sanitize_possible_rotors(const vector<string>& vec) {
    using Enigma::rotor_name;
    vector<int> result = {};
    
    for (const string& arg : vec) {
        const auto iter = find(rotor_name.begin(), rotor_name.end(), arg);
        if (iter != rotor_name.end())
            result.push_back(iter - rotor_name.begin());
        else
            result.push_back(stoi(arg)%8);
    }
    return result;
}

// Translate string names into into indexes for the vector<Fitness_Function*> .
vector<int> get_fitness_functions(const vector<string>& vec) {  
    const vector<string> ff = {"ioc", "letters", "bigrams", "trigrams", "quadgrams", "known"};
    
    if (vec.size() < 3) {
        const int index = find(ff.begin(), ff.end(), vec[0]) - ff.begin();
        if (index != ff.size())
            return {index,index,index}; 
    } else {
        const int index0 = find(ff.begin(), ff.end(), vec[0]) - ff.begin();
        const int index1 = find(ff.begin(), ff.end(), vec[1]) - ff.begin();
        const int index2 = find(ff.begin(), ff.end(), vec[2]) - ff.begin();
        if (index0 != ff.size() && index1 != ff.size() && index2 != ff.size())
            return {index0,index1,index2}; 
    }
    return {0,0,3};
}



void print_help() { 
cout << "This is a terminal utility to decrypt enigma enciphered text.\n";
cout << "\n";
cout << "All modes require \"--ciphertext [filepath]\" CLI arguments to point to the text file containing the ciphertext.\n";
cout << "\n";
cout << "Modes:\n";
cout << "\tWithout specifying a mode in the first CLI argument, the program will run the below functions start to finish and return the best key it finds.\n"; 
cout << "\n";
cout << "\t\"--rotors\" or \"--names\":\n";
cout << "The program will brute-force which rotors are used, in what order, and in which positions.\n";
cout << "\n";
cout << "\t\"--pos\" or \"--positions\":\n";
cout << "The program will brute-force the best positions for the rotors given in the input enigma key.\n";
cout << "\n";
cout << "\t\"--rings\" or \"--notches\":\n";
cout << "The program will brute-force the best ring-settings of the given rotors, while keeping the internal wiring as is by moving the position to cancel out the effect on the internals of the rotor. In short, brute-forces the best notch position.\n";
cout << "\n";
cout << "\"--plugs\" or \"--plugboard\":\n";
cout << "The program will brute-force additional plugs for the plugboard.\n";
cout << "\n";
cout << "\"--encrypt\" or \"--enigma\":\n";
cout << "Returns the given ciphertext after encrypting it with the given key.\n";
cout << "\n";
cout << "Optional Arguments:\n";
cout << "\n";
cout << "\t\"--key [Engima Key]\"\n";
cout << "should be followed by an Enigma key formatted as a Roman numeral followed by two capital letters, three times, followed by pairs of capital letters.\n";
cout << "\"--key IV U F III T K V D Z AZ GR HK\"\n";
cout << "The left rotor is IV, its position visible atop the machine is U, and the dot identifying the ring setting is over F. The middle rotor is III, position T, ring setting K. The right rotor is V, position D, ring setting Z. The plugboard has connected A to Z, G to R, and H to K.\n";
cout << "\n";
cout << "\t\"--amount [int]\"\n";
cout << "should be followed by an integer value specifying how many results you want printed.\n";
cout << "\n";
cout << "\t\"--fitness\"\n";
cout << "should be followed by one (or three if ran in default mode) of the following:\n";
cout << "\"ioc\", \"letters\", \"bigrams\", \"trigrams\", \"quadgrams\", \"known\"\n";
cout << "The second to the fifth require that the program be executed next to (in the same directory as) the folder that contains english data. The last requires another CLI argument \"--known_plaintext\".\n";
cout << "\n";
cout << "\t\"--known_plaintext [filepath]\"\n";
cout << "should be followed by a filepath to a text file containing whatever of the ciphertext is known. For example if the first three letters are known, as well as the 12th through 15th, then the file should contain:\n";
cout << "\"THE________ARE\"\n";
cout << "\n";
cout << "\t\"--possible_rotors [numerals]\"\n";
cout << "should be followed by one to eight Roman numerals to specify which rotors should be attempted.\n";

}

