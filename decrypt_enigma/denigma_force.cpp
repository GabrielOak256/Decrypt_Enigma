// The definitions of the brute-forcing functions against the Enigma ciphertext.

#include"denigma.hpp"
#include"encrypt_enigma/enigma.hpp"

#include<vector>
#include<string>
#include<algorithm>
//#include<iostream> 

namespace Denigma {
    using std::vector;
    using std::string;
    

// An all-in-one function to run the others. 
Enigma::Key force_ciphertext(const string& ciphertext, const vector<int>& possible_rotors, const Fitness_Function& fitness0, const Fitness_Function& fitness1, const Fitness_Function& fitness2) {
    Enigma::Key result = force_rotor_notch(ciphertext, force_rotor_name(ciphertext, possible_rotors, 1, fitness0)[0].key, 1, fitness1)[0].key;
    result.plugs = force_plugboard(ciphertext, result, fitness2)[0].plugs;
    return result; 
} // force_ciphertext
 

// Brute-force which rotors were used, in what order, and in what position, 
// according to the given fitness function object. 
vector<Scored_Key> force_rotor_name(const string& ciphertext, const vector<int>& possible_rotors, const int& amount, const Fitness_Function& fitness, const vector<pair<int,int>>& plugboard_key) {
    if (amount <= 0) { return {}; }
    
    // Construct an "amount" length vector, with default keys scored worse than
    // the score of any message we're likely to get with negative logarithms.
    vector<Scored_Key> result(amount, Scored_Key{-1000000000.0, Enigma::Key{}});
    
    // Set aside memory for these before the loop starts to save time. 
    Scored_Key sckey;
    auto rev_iter = result.rbegin();
    
    // For every name combination,
    for (const int& l : possible_rotors) {
    for (const int& m : possible_rotors) {
        if (m == l) { continue; }
    for (const int& r : possible_rotors) {
        if (r == m || r == l) { continue; }
        
        // Get its best positions and score of that, 
        sckey = Scored_Key{force_rotor_pos(ciphertext, {{l,m,r}}, 1, fitness, plugboard_key)[0], plugboard_key};
        // If that score is better than any other best, add it.
        rev_iter = upper_bound(result.rbegin(), result.rend(), sckey, [](const auto& a, const auto& b){return a.score < b.score;}); 
        if (rev_iter != result.rbegin()) {
            result.insert(((rev_iter+1).base())+1, sckey);
            result.pop_back();
            // Uncomment this to see the program as it's running. 
            // for (auto k : result)
            //     std::cout << string(k) << "\n";
            // std::cout << "\n";
        }
        // Consider the above to be a bodged implementation of what a standard 
        // library 'fixed-length' priority_queue object would do. 
        
    }}} 
    return result;
} // force_rotor_name


// Given the names of the rotors (which ones in which order) find positions. 
vector<Scored_Rotors> force_rotor_pos(const string& ciphertext, const array<int,3>& rotor_names, const int& amount, const Fitness_Function& fitness, const vector<pair<int,int>>& plugboard_key) {
    if (amount <= 0) { return {}; }
    
    vector<Scored_Rotors> result(amount, {-1000000000.0, {{ {rotor_names[0],0,0}, {rotor_names[1],0,0}, {rotor_names[2],0,0} }} });
    Enigma::Machine enigma({{ {rotor_names[0],0,0}, {rotor_names[1],0,0}, {rotor_names[2],0,0} }}, plugboard_key);
    
    double score = 0.0;
    auto rev_iter = result.rbegin();
    
    for (int l_pos = 0; l_pos < 26; ++l_pos) {
    for (int m_pos = 0; m_pos < 26; ++m_pos) {
    for (int r_pos = 0; r_pos < 26; ++r_pos) {  
        score = fitness(enigma.set_rotors_pos({{l_pos, m_pos, r_pos}}).fast_encrypt(ciphertext));
        rev_iter = upper_bound(result.rbegin(), result.rend(), score, [](const double& a, const Scored_Rotors& b){return a < b.score;}); 
        if (rev_iter != result.rbegin()) {
            result.insert(((rev_iter+1).base())+1, Scored_Rotors{score, {{ {rotor_names[0],l_pos,0}, {rotor_names[1],m_pos,0}, {rotor_names[2],r_pos,0} }} });
            result.pop_back();
            // Uncomment this to see the program as it's running. 
            // for (auto k : result)
            //     std::cout << string(Scored_Key{k,plugboard_key}) << "\n";
            // std::cout << "\n";
        }
    }}}
    return result; 
} // force_rotor_pos


// By changing the ring setting and adjusting the position to cancel out the 
// effect on the internal wiring, we change where the notch on each ring is. 
// Brute-force which of those notch positions gives the best result. 
vector<Scored_Key> force_rotor_notch(const string& ciphertext, const Enigma::Key& key, const int& amount, const Fitness_Function& fitness) {
    if (amount <= 0) { return {}; }
    
    vector<Scored_Key> result(amount, Scored_Key{-1000000000.0, Enigma::Key{}});
    Enigma::Machine enigma(key);
    
    Scored_Key sckey;
    auto rev_iter = result.rbegin();
    array<array<int,3>,3> rotors_key = {}; // to save me from typing it out twice
    
    for (int i = 0; i < 26; ++i) {
    for (int j = 0; j < 26; ++j) {
        rotors_key = {{ key.rotors[0], {key.rotors[1][0], (key.rotors[1][1]+i)%26, i}, {key.rotors[2][0], (key.rotors[2][1]+j)%26, j} }};
        sckey = Scored_Key{fitness(enigma.set_rotors(rotors_key).fast_encrypt(ciphertext)), Enigma::Key{rotors_key,key.plugs}};
        rev_iter = upper_bound(result.rbegin(), result.rend(), sckey, [](const auto& a, const auto& b){return a.score < b.score;});
        if (rev_iter != result.rbegin()) {
            result.insert(((rev_iter+1).base())+1, sckey);
            result.pop_back();
            // Uncomment this to see the program as it's running. 
            // for (auto k : result)
            //     std::cout << string(k) << "\n";
            // std::cout << "\n";
        }
    }}
    return result; 
} // force_rotor_notch

        
// Improve the text by adding one plug at a time. 
vector<Scored_Plugboard> force_plugboard(const string& ciphertext, const Enigma::Key& key, const Fitness_Function& fitness) {
    vector<Scored_Plugboard> result = {}; 
    Enigma::Machine enigma(key);
    
    // We can't plug two cords into the same plug. This stores open plugs.
    vector<int> alphabet = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
    
    // Get a baseline.
    result.push_back(Scored_Plugboard{fitness(enigma.set_plugs({}).fast_encrypt(ciphertext)), {}});
    
    // If we were given plugs to start with, score those. 
    vector<pair<int,int>> plugs = {};
    for (const auto& p : key.plugs) {  
        plugs.push_back(p);
        result.push_back(Scored_Plugboard{fitness(enigma.set_plugs(plugs).fast_encrypt(ciphertext)), plugs});
        alphabet.erase(std::find(alphabet.begin(), alphabet.end(), p.first));
        alphabet.erase(std::find(alphabet.begin(), alphabet.end(), p.second));
    }
    
    // Set aside memory for these before the loop starts to save time. 
    pair<int,int> plug = {};
    pair<int,int> best_plug = {};
    double score = 0.0;
    double best_score = -1000000000.0;
    
    // "> 1" because alphabet might be odd length if input contained "AB CB HW"
    while (alphabet.size() > 1) { 
    // Brute force every possible plug, 
    for (auto iter = alphabet.begin(); iter < alphabet.end(); ++iter) {
    for (auto jter = iter+1;           jter < alphabet.end(); ++jter) {
        plug = std::make_pair(*iter,*jter);
        plugs.push_back(plug);
        double score = fitness(enigma.set_plugs(plugs).fast_encrypt(ciphertext));
        plugs.pop_back();
        if (best_score < score) {
            best_score = score;
            best_plug  = plug;
        }   
    }}
    // If the best new plug is an improvement, add it. Otherwise stop.
    if (best_score > result.back().score) {
        plugs.push_back(best_plug);
        result.push_back(Scored_Plugboard{best_score, plugs});
        alphabet.erase(std::find(alphabet.begin(), alphabet.end(), best_plug.first));
        alphabet.erase(std::find(alphabet.begin(), alphabet.end(), best_plug.second));
    } else { break; }
    }
    
    return result; 
} // force_plugboard



} // namespace Denigma





