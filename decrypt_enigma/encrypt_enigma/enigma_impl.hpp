// The interface for the implementers of the Enigma library.

#include"enigma.hpp" // for consistency check

namespace Enigma_impl {
    using namespace Enigma;
    
    
    
    // Machine
    
    constexpr array<int,26> alphabet = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
    
    // A helper function to initialize Machine::plugboard.
    array<int,26> translate_plugs(const vector<pair<int,int>>& plugs_key);
    
    
    
    // Rotors 
    
    // rotor_wiring[letter] = the letter that comes out in pos=0 and ring=0.
    constexpr array<array<int,26>,8> rotor_wiring = {{
    {4,10,12,5,11,6,3,16,21,25,13,19,14,22,24,7,23,20,18,15,0,8,1,17,2,9}, 
    {0,9,3,10,18,8,17,20,23,1,11,7,22,19,12,2,16,6,25,13,15,24,5,21,14,4},
    {1,3,5,7,9,11,2,15,17,19,23,21,25,13,24,4,8,22,6,0,10,12,20,18,16,14},
    {4,18,14,21,15,25,9,0,24,16,20,8,17,7,23,11,13,5,19,6,10,3,2,12,22,1},
    {21,25,1,17,6,8,19,24,20,15,18,3,13,7,11,23,0,22,12,9,16,14,5,4,2,10},
    {9,15,6,21,14,20,12,5,24,16,1,4,13,7,25,17,3,10,0,18,23,11,8,2,19,22},
    {13,25,9,7,6,17,2,23,12,24,18,22,1,14,20,5,0,8,21,11,15,4,10,16,3,19},
    {5,10,16,7,19,11,23,14,2,1,9,18,15,3,25,17,0,12,4,22,13,8,20,24,6,21}}};
    
    // reverse_rotor_wiring[rotor_wiring[letter]] = letter;
    constexpr array<array<int,26>,8> reverse_rotor_wiring = {{
    {20,22,24,6,0,3,5,15,21,25,1,4,2,10,12,19,7,23,18,11,17,8,13,16,14,9},
    {0,9,15,2,25,22,17,11,5,1,3,10,14,19,24,20,16,6,4,13,7,23,12,8,21,18},
    {19,0,6,1,15,2,18,3,16,4,20,5,21,13,25,7,24,8,23,9,22,11,17,10,14,12},
    {7,25,22,21,0,17,19,13,11,6,20,15,23,16,2,4,9,12,1,18,10,3,24,14,8,5},
    {16,2,24,11,23,22,4,13,5,19,25,14,18,12,21,9,20,3,10,6,8,0,17,15,7,1},
    {18,10,23,16,11,7,2,13,22,0,17,21,6,12,4,1,9,15,19,24,5,3,25,20,8,14},
    {16,12,6,24,21,15,4,3,17,2,22,19,8,0,13,20,23,5,10,25,14,18,11,7,9,1},
    {16,9,8,13,18,0,24,3,21,10,1,5,17,20,7,12,2,15,11,4,22,25,19,6,23,14}}};
    
    // Cleans an int into 0 <= d < 26. Fails for d < -26. 
    // "for (; d < 0; d += 26);" would be less error prone than
    // "d + 26", but slower.
    constexpr inline int cln(const int& d) { return (d + 26)%26; }
    
    // A shorthand for a pointer to a function taking int and returning bool.
    typedef bool (*int_test)(const int& d); 
    
    
              
    // Key 
           
    // A helper function to initialize Key.plugs .
    void validate_plugs(vector<pair<int,int>>& plugs);
    
}
