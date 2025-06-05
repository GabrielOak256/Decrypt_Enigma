// A terminal utility to encrypt text with a virtual Enigma Machine. 

// g++ main.cpp enigma.hpp enigma_impl.hpp enigma_machine.cpp enigma_rotors.cpp enigma_key.cpp -o encrypt_enigma

#include"enigma.hpp"
#include<iostream>

using namespace std;

int main(int argc, char* argv[]) { // + 1, Remove program name.
    const vector<string> args = {argv + 1, argv + argc}; 
    
    Enigma::Key key(args);
    Enigma::Machine enigma(key.rotors, key.plugs);
    
    string plaintext = "";
    char c = '\0';    
    for (cin.get(c); !cin.eof(); cin.get(c))
        plaintext += c;
    plaintext.pop_back();  // Remove EOF character.
    
    cout << "\n\n" << string(key) << "\n" << enigma.encrypt(plaintext) << endl;
    
    return 0;
}
