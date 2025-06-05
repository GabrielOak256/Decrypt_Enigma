// The implementation of the Enigma library pertaining to the Key. 
// Which handles user input. 

#include"enigma_impl.hpp"
#include<iostream>
#include<algorithm> // rotate()
#include<set>
#include<algorithm>

namespace Enigma {
    using namespace Enigma_impl;
    
    using std::vector;
    using std::string;
    using std::pair;
    
    
    // Default Construct Key
    Key::Key() { 
        rotors = {{ {0,0,0}, {1,0,0}, {2,0,0} }};
        plugs = {};
    }
    
    // Construct Key from vector<string>. 
    Key::Key(const vector<string>& args) {
        rotors = {{ {0,0,0}, {1,0,0}, {2,0,0} }}; 
        if (args.size() < 9) 
            std::cerr << "Too few rotor settings, using default rotors." << std::endl;
        else  
            for (int i = 0; i < 9; ++i)
                if (i % 3) { // i != 0,3,6
                    if (isalpha(args[i][0])) { rotors[i/3][i%3] = args[i][0]-65; }
                    else { rotors[i/3][i%3] = std::stoi(args[i])%26; }
                } else {
                    const int j = std::find(rotor_name.begin(), rotor_name.end(), args[i]) - rotor_name.begin();
                    if (j != rotor_name.size()) { rotors[i/3][0] = j; }
                    else { rotors[i/3][0] = std::stoi(args[i])%26; }
                }
        // Initialize the plugs
        for (int i = 9; i < args.size() && i < 22 && args[i].size() == 2; ++i) // 22 = 9 + (26/2)
            if (isalpha(args[i][0]) && isalpha(args[i][1])) 
                plugs.push_back(std::make_pair(int(toupper(args[i][0])-65), int(toupper(args[i][1])-65)));
        validate_plugs(plugs);
    }
    
    
    Key::Key(const array<array<int,3>,3>& rotors, const vector<pair<int,int>>& plugs): rotors{rotors}, plugs{plugs} {}
    
    
    // Defines conversion of Key to string. 
    Key::operator string() const {
        std::string result = "";
        for (int i = 0; i < 3; ++i) 
            result = result + string(rotor_name[rotors[i][0]]) + " pos:" + char(rotors[i][1]+65) + " ring:" + char(rotors[i][2]+65) + " ";
        for (const auto& plug : plugs) 
            result = result + char(plug.first+65) + "-" + char(plug.second+65) + " ";
        result.pop_back(); // Remove trailing " ".
        return result;
    } 
    
    
    
} // namespace Enigma



// A helper function to initialize Key.plugs .
void Enigma_impl::validate_plugs(vector<pair<int,int>>& plugs) {
    std::set<int> used = {}; 
    for (auto& p : plugs) {
        if (used.find(p.first) != used.end() || used.find(p.second) != used.end() || p.first == p.second) {
            std::cerr << "Invalid plugs, defaulting to no plugs." << std::endl;
            plugs = {};
            return;
        }
        used.insert(p.first);
        used.insert(p.second);
        if (p.first > p.second)
            std::swap(p.first, p.second);
    } 
    sort(plugs.begin(), plugs.end(), [](const auto& a, const auto& b){return a.first < b.first;}); 
}



