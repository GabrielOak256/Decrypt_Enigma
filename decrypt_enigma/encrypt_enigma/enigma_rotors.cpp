// The implementation of the Enigma library pertaining to the Rotors.
// A virtual Rotor bank component of the Enigma machine. 

#include"enigma_impl.hpp"
#include<algorithm> // rotate()

namespace Enigma_impl {
    // Rotors will expose their left neighbor (and themselves) to rotation
    // when going from:                     // The latter 3 are M > N or Z > A
    constexpr bool I_notch    (const int& d) { return d == 16; }      // Q > R
    constexpr bool II_notch   (const int& d) { return d == 4; }       // E > F
    constexpr bool III_notch  (const int& d) { return d == 21; }      // V > W
    constexpr bool IV_notch   (const int& d) { return d == 9; }       // J > K
    constexpr bool V_notch    (const int& d) { return d == 25; }      // Z > A
    constexpr bool VI_notch   (const int& d) { return d == 12 || d == 25; } 
    constexpr bool VII_notch  (const int& d) { return d == 12 || d == 25; } 
    constexpr bool VIII_notch (const int& d) { return d == 12 || d == 25; } 
    
    // A collection of notch tests.
    int_test notch_test[8] = {I_notch, II_notch, III_notch, IV_notch, 
                              V_notch, VI_notch, VII_notch, VIII_notch};
}



namespace Enigma {
    using namespace Enigma_impl;
    
    using std::vector;
    using std::string;
    using std::array;
    using std::pair;
    
    
    
    // Class Constructor of Rotors 
    Machine::Rotors::Rotors(const array<array<int,3>,3>& k = {{{0,0,0},{1,0,0},{2,0,0}}}): rotor_l(k[0]), rotor_m(k[1]), rotor_r(k[2]) {}
    
    // Sets the member values of Rotors.
    void Machine::Rotors::set_key(const array<array<int,3>,3>& rotors_key) {
        rotor_l.set_key(rotors_key[0]);
        rotor_m.set_key(rotors_key[1]);
        rotor_r.set_key(rotors_key[2]);
    }
    
    void Machine::Rotors::set_pos(const array<int,3>& positions) {
        rotor_l.pos = positions[0];
        rotor_m.pos = positions[1];
        rotor_r.pos = positions[2];
    }
    
    array<int,3> Machine::Rotors::get_pos() { return {rotor_l.pos, rotor_m.pos, rotor_r.pos}; }
    
    
    
    // Rotate rotors on keypress.
    void Machine::Rotors::step() {
        if((*notch_test[rotor_m.name])(rotor_m.pos)) {
            ++rotor_l.pos %= 26;
            ++rotor_m.pos %= 26; // Double Stepping 
        }
        if((*notch_test[rotor_r.name])(rotor_r.pos)) {
            ++rotor_m.pos %= 26; // Won't overlap with above ++rotor1.pos
        }
        ++rotor_r.pos %= 26;
    } 
    
    // From right to left rotor, cleanly index to its rotation, and read.
    int Machine::Rotors::read (const int& d) { 
        return cln( rotor_l.f_wiring[cln( rotor_m.f_wiring[cln( rotor_r.f_wiring[cln( d + rotor_r.pos )] - rotor_r.pos + rotor_m.pos )] - rotor_m.pos + rotor_l.pos )] - rotor_l.pos ); 
    }
    
    // reverse of the above function
    int Machine::Rotors::daer (const int& d) { 
        return cln( rotor_r.r_wiring[cln( rotor_m.r_wiring[cln( rotor_l.r_wiring[cln( d + rotor_l.pos )] - rotor_l.pos + rotor_m.pos )] - rotor_m.pos + rotor_r.pos )] - rotor_r.pos ); 
    }
    
    
    
    // Class Constructor of a Rotor within Rotors
    Machine::Rotors::Rotor::Rotor(const array<int,3>& rotor_key) { set_key(rotor_key); }
    
    // Sets the member values of a Rotor.
    // Expected arguments are name, position, ring setting.
    void Machine::Rotors::Rotor::set_key(const array<int,3>& rotor_key) {
        name = rotor_key[0];
        pos = rotor_key[1];
        f_wiring = rotor_wiring[name];
        r_wiring = reverse_rotor_wiring[name];
        
        // precalulate f_wiring[d - ring_setting] into f_wiring
        std::rotate(f_wiring.begin(), f_wiring.begin() + 26-rotor_key[2], f_wiring.end());
        std::rotate(r_wiring.begin(), r_wiring.begin() + 26-rotor_key[2], r_wiring.end());
        
        // precalulate f_wiring[d - ring_setting] + ring_setting into f_wiring
        for (int& d : f_wiring)
            d = (d + rotor_key[2])%26;
        for (int& d : r_wiring)
            d = (d + rotor_key[2])%26;
    }
    
    
    
} // namespace Enigma



// Rotor Wirings: 

// ABCDEFGHIJKLMNOPQRSTUVWXYZ // Plain

// EKMFLGDQVZNTOWYHXUSPAIBRCJ // I
// AJDKSIRUXBLHWTMCQGZNPYFVOE // II
// BDFHJLCPRTXVZNYEIWGAKMUSQO // III
// ESOVPZJAYQUIRHXLNFTGKDCMWB // IV
// VZBRGITYUPSDNHLXAWMJQOFECK // V
// JPGVOUMFYQBENHZRDKASXLICTW // VI
// NZJHGRCXMYSWBOUFAIVLPEKQDT // VII
// FKQHTLXOCBJSPDZRAMEWNIUYGV // VIII         

// UWYGADFPVZBECKMTHXSLRINQOJ // reverse I
// AJPCZWRLFBDKOTYUQGENHXMIVS // reverse II
// TAGBPCSDQEUFVNZHYIXJWLRKOM // reverse III
// HZWVARTNLGUPXQCEJMBSKDYOIF // reverse IV
// QCYLXWENFTZOSMVJUDKGIARPHB // reverse V
// SKXQLHCNWARVGMEBJPTYFDZUIO // reverse VI
// QMGYVPEDRCWTIANUXFKZOSLHJB // reverse VII
// QJINSAYDVKBFRUHMCPLEWZTGXO // reverse VIII 


// "Double Stepping"
// The Enigma's keyboard lifts a lever with a pawl (tooth) for each rotor. 
// The rightmost is always exposed to its pawl, and moves every keypress.
// A rotor covers its left neighbor, except for a notch that exposes the 
// left neighbor for a single keypress. When this notch is exposed, and 
// the pawl can make contact, it pushes both rotors (the one lifted at the 
// notch and the one whose ratchet is now exposed). This is trivial for the 
// rightmost rotor, as its ever exposed rachet will be incremented anyway, 
// but noteworthy for the middle rotor. The right rotor increments the 
// middle into exposing the middle's notch that would've covered the left.
// Now the middle should not move for another 25 keypresses. However, next
// keypress, both the left and middle will be incremented. 


// Why are the rotor positions being added and subracted as they're read?
// Each Rotor's wiring member is a static chart (with the ring setting 
// information built-in). To read it as if it was a truly spinning rotor, we 
// must rotate that chart appropriately, read which offset that wire contact 
// provides, and then de-rotate that output back to the chart we actually have.


// What is the "ring setting"?
// Each rotor has an inner part with the wiring and contacts, and an outer part 
// with the position label and notch. The ring setting is the offset between 
// these two components. It is indicated by a dot that is next to A by default.
// In this program, +3 means the dot is now A>B>C>D, -2 means A>Z>Y.


