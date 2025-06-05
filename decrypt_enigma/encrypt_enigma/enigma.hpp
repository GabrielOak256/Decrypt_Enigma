// A virtual implementation of the 3-rotor Enigma machine. 
// The interface for the users of the Enigma library.

#ifndef ENIGMA_CLASS_05_13_2025
#define ENIGMA_CLASS_05_13_2025

#include<vector>
#include<string>
#include<array> 

namespace Enigma {

    using std::vector;
    using std::string;
    using std::array;
    using std::pair;
    
    
// The different rotors were labelled in Roman numerals. 
constexpr array<char[5],8> rotor_name = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII"};
    
    
struct Key {
    array<array<int,3>,3> rotors;
    vector<pair<int,int>> plugs = {};
    Key();
    Key(const vector<string>& args);
    Key(const array<array<int,3>,3>& rotors, const vector<pair<int,int>>& plugs);
    operator string() const; 
}; // struct Key
    
    

class Machine {
public:
    
    Machine(const Key& key);
    Machine(const array<array<int,3>,3>& rotors_key = {{ {0,0,0}, {1,0,0}, {2,0,0} }}, const vector<pair<int,int>>& plugboard_key = {});
    
    Machine& set_key(const array<array<int,3>,3>& rotors_key, const vector<pair<int,int>>& plugboard_key);
    Machine& set_rotors(const array<array<int,3>,3>& rotors_key);
    Machine& set_plugs(const vector<pair<int,int>>& plugs_key); 
    Machine& set_rotors_pos(const array<int,3>& positions); 
    array<int,3> get_rotors_pos(); // Because pos changes during brute-force
    
    int fast_encrypt(const int& d);
    int encrypt(const int& d);
    char fast_encrypt(const char& c);
    char encrypt(const char& c);
    string fast_encrypt(const string& str);
    string encrypt(const string& str);
    
private:
    
    class Rotors { 
    public: 
        Rotors(const array<array<int,3>,3>& rotors_key);
        void set_key(const array<array<int,3>,3>& rotors_key); 
        void set_pos(const array<int,3>& positions); // brute-force tests pos separately from ring
        array<int,3> get_pos(); // Because pos changes during brute-force
        
        void step();
        int read(const int& d); 
        int daer(const int& d); // int reverse_read(const int& d);
        
    private: 
        class Rotor {
        public:
            Rotor(const array<int,3>& rotor_key);    
            void set_key(const array<int,3>& rotor_key); 
        
            array<int,26> f_wiring; // R/W exposed to Rotors for fast read
            array<int,26> r_wiring; // instead of get(), and reinitialization
            int name;               // during brute-force.
            int pos; 
        }; // Machine::Rotors::Rotor
        Rotor rotor_l; 
        Rotor rotor_m; 
        Rotor rotor_r; 
    }; // Machine::Rotors
    Rotors rotors;
    
    constexpr static int reflector[26] = {24, 17, 20, 7, 16, 18, 11, 3, 15, 23, 13, 6, 14, 10, 12, 8, 4, 1, 5, 25, 2, 22, 21, 9, 0, 19};   
    
    array<int,26> plugboard;  // non const for brute-force reinitialization
}; // class Machine
    
    
} // namespace Enigma


// Reflector wirings:
// A: EJMZALYXVBWFCRQUONTSPIKHGD // Was phased out before WWII.
// B: YRUHQSLDPXNGOKMIEBFZCWVJAT // The only one implemented here.
// C: FVPJIAOYEDRZXWGCTKUQSBNMHL // Wasn't used for long.
// [4,9,12,25,0,11,24,23,21,1,22,5,2,17,16,20,14,13,19,18,15,8,10,7,6,3] 
// [24,17,20,7,16,18,11,3,15,23,13,6,14,10,12,8,4,1,5,25,2,22,21,9,0,19]
// [5,21,15,9,8,0,14,24,4,3,17,25,23,22,6,2,19,10,20,16,18,1,13,12,7,11] 

#endif
