// The implementation of the Enigma library pertaining to the Machine.

#include"enigma_impl.hpp"

namespace Enigma {
    using namespace Enigma_impl;
    
    using std::vector;
    using std::string;
    using std::array;
    using std::pair;
    
    
    
    // Machine's class Constructor
    Machine::Machine(const Key& key): rotors(key.rotors), plugboard{translate_plugs(key.plugs)} {}
    
    Machine::Machine(const array<array<int,3>,3>& rotors_key, const vector<pair<int,int>>& plugboard_key): rotors(rotors_key), plugboard{translate_plugs(plugboard_key)} {}
    
    
    // Functions to reinitialize Machine during brute-force. 
    Machine& Machine::set_key(const array<array<int,3>,3>& rotors_key, const vector<pair<int,int>>& plugboard_key) {
        set_rotors(rotors_key);
        set_plugs(plugboard_key);
        return *this;
    }
    Machine& Machine::set_rotors(const array<array<int,3>,3>& rotors_key) { rotors.set_key(rotors_key); return *this; }
    Machine& Machine::set_plugs(const vector<pair<int,int>>& plugs_key) { plugboard = translate_plugs(plugs_key); return *this; }
    Machine& Machine::set_rotors_pos(const array<int,3>& positions) { rotors.set_pos(positions); return *this; }
    array<int,3> Machine::get_rotors_pos() { return rotors.get_pos(); }
    
    
    
    // Functions to encrypt data of relevant types. 
    
    int Machine::fast_encrypt(const int& d) { 
        rotors.step();
        return plugboard[rotors.daer(reflector[rotors.read(plugboard[d])])]; 
    } 
    
    int Machine::encrypt(const int& d) { return fast_encrypt(d%26); }
    
    char Machine::fast_encrypt(const char& c) { return fast_encrypt(int(c)-65)+65; } 
    
    char Machine::encrypt(const char& c) { return (isalpha(c)) ? (fast_encrypt(int(toupper(c))-65)+65) : c; }  
    
    string Machine::fast_encrypt(const string& str) { // Expects upper,alpha input.
        string result = ""; 
        for (const char& c : str) 
            result += fast_encrypt(char(c)); 
        return result;
    }
    
    string Machine::encrypt(const string& str) {
        string result = ""; 
        for (const char& c : str) 
            if (isalpha(c)) 
                result += fast_encrypt(char(toupper(c))); 
        return result;
    }
    
    
} // namespace Enigma



// A helper function to initialize Machine::plugboard.
std::array<int,26> Enigma_impl::translate_plugs(const vector<pair<int,int>>& plugs_key) {
    array<int,26> result = alphabet;
    for (const auto& p : plugs_key) 
        std::swap(result[p.first], result[p.second]);
    return result;
}

