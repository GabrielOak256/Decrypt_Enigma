// The definitions of Key objects used by the decryptor to simplify data 
// transfer and presentation. 

#include"denigma.hpp"
#include"encrypt_enigma/enigma.hpp"

#include<string>

namespace Denigma {
    using std::string;
    
    Scored_Key::Scored_Key(): score{0.0}, key() {}
    
    Scored_Key::Scored_Key(const double& score, const vector<string>& key): score{score}, key(key) {}
    Scored_Key::Scored_Key(const double& score, const Enigma::Key& key): score{score}, key(key) {}
    
    Scored_Key::Scored_Key(const Scored_Rotors& scored_rotors, const vector<pair<int,int>>& plugs): score{scored_rotors.score}, key(scored_rotors.rotors, plugs) {}
    
    Scored_Key::Scored_Key(const array<array<int,3>,3>& rotors, const Scored_Plugboard& scored_plugs): score{scored_plugs.score}, key(rotors, scored_plugs.plugs) {}
    
    Scored_Key::operator string() const { return string(key) + "\t" + std::to_string(score); }
}
