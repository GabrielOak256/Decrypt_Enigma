// The interface for users of the enigma decryption library.

#ifndef DENIGMA_CLASS_05_23_2025
#define DENIGMA_CLASS_05_23_2025

#include"encrypt_enigma/enigma.hpp"

#include<unordered_map>

namespace Denigma {

    using std::vector;
    using std::string;
    using std::pair;
    using std::array;
    using std::unordered_map;
    
struct Scored_Rotors {
    double score = 0.0;
    array<array<int,3>,3> rotors = {{ {0,0,0}, {1,0,0}, {2,0,0} }};
};
    
struct Scored_Plugboard {
    double score = 0.0;
    vector<pair<int,int>> plugs = {};
};
    
struct Scored_Key {
    double score;
    Enigma::Key key;
        
    Scored_Key();
    Scored_Key(const double& score, const vector<string>& key); 
    Scored_Key(const double& score, const Enigma::Key& key); 
    Scored_Key(const Scored_Rotors& scored_rotors, const vector<pair<int,int>>& plugs); 
    Scored_Key(const array<array<int,3>,3>& rotors, const Scored_Plugboard& scored_plugs); 
    operator string() const;
};
    
struct Fitness_Function {
    virtual double operator()(const string& str) const =0;
};

struct IoC : public Fitness_Function {
    double operator()(const string& str) const override;        
};
    
struct Known : public Fitness_Function { // Compares input with known plaintext 
    Known(const string& plaintext);      // Doesn't exploit the Reflector
    const string plaintext;
    double operator()(const string& str) const override;  
};
    
struct Gram_Fitness_Function : public Fitness_Function {
    Gram_Fitness_Function(const int& len);
    const unordered_map<string,double> data = {};
    double operator()(const string& str, const int& len) const;
};
    
struct Letters : public Gram_Fitness_Function {
    Letters();
    double operator()(const string& str) const override;
};
    
struct Bigrams : public Gram_Fitness_Function {
    Bigrams();
    double operator()(const string& str) const override;
};
    
struct Trigrams : public Gram_Fitness_Function {
    Trigrams();
    double operator()(const string& str) const override;
};
    
struct Quadgrams : public Gram_Fitness_Function {
    Quadgrams();
    double operator()(const string& str) const override;
};   

    

Enigma::Key force_ciphertext(const string& ciphertext, const vector<int>& possible_rotors = {0,1,2,3,4}, const Fitness_Function& fitness0 = IoC(), const Fitness_Function& fitness1 = IoC(), const Fitness_Function& fitness2 = IoC());

vector<Scored_Key> force_rotor_name(const string& ciphertext, const vector<int>& possible_rotors = {0,1,2,3,4}, const int& amount = 1, const Fitness_Function& fitness = IoC(), const vector<pair<int,int>>& plugboard_key = {});

vector<Scored_Rotors> force_rotor_pos(const string& ciphertext, const array<int,3>& rotor_names, const int& amount = 1, const Fitness_Function& fitness = IoC(), const vector<pair<int,int>>& plugboard_key = {});

vector<Scored_Key> force_rotor_notch(const string& ciphertext, const Enigma::Key& key, const int& amount = 1, const Fitness_Function& fitness = IoC());
    
vector<Scored_Plugboard> force_plugboard(const string& ciphertext, const Enigma::Key& key, const Fitness_Function& fitness);

    

unordered_map<string,double> initialize_gram(const int& len);

} // namespace Denigma

#endif
