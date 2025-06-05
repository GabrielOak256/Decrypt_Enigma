// The definitions of the fitness function objects used by the decryptor.

#include"denigma.hpp"

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<algorithm>

namespace Denigma {
    
    using std::vector;
    using std::string;
    using std::ifstream;
    
    // Index of Coincidence 
    double IoC::operator()(const string& str) const {
        // Get amount of each letter.
        vector<int> counts(26,0);
        for (const auto& c : str)
            ++counts[c-65];
        // Calculate sum of numerators of probability for each letter.
        double numerator = 0.0; 
        for (const auto& d : counts) 
            numerator += (d * (d - 1));
        // Apply denominator of each probability and return. 
        return (numerator / (str.size() * (str.size()-1)));
    }      
    
    
    // Known Plaintext 
    
    Known::Known(const string& plaintext): plaintext{plaintext} {}
    
    double Known::operator()(const string& str) const {
        double result = 0.0;
        const int len = std::min(str.size(), plaintext.size());
        for (int i = 0; i < len; ++i)
            if (str[i] == plaintext[i])
                ++result;
        return result; 
    }
    
    
    // Letters, Bigrams, Trigrams, Quadgrams 
    
    Gram_Fitness_Function::Gram_Fitness_Function(const int& len): data{initialize_gram(len)} {}
    
    double Gram_Fitness_Function::operator()(const string& str, const int& len) const {
        if (str.size() < len) { return 0.0; }
        double result = 0.0;
        for (int i = 0; i < str.size() - (len-1); ++i) {
            auto iter = data.find(str.substr(i,len));
            if (iter != data.end())
                result += (*iter).second; 
            else
                result += -7.5; // The bottom of our english data
        }
        return result;
    }
    
    Letters::Letters(): Gram_Fitness_Function(1) {}
    Bigrams::Bigrams(): Gram_Fitness_Function(2) {}
    Trigrams::Trigrams(): Gram_Fitness_Function(3) {}
    Quadgrams::Quadgrams(): Gram_Fitness_Function(4) {}
    
    double Letters::operator()(const string& str) const { return Gram_Fitness_Function::operator()(str, 1); }
    double Bigrams::operator()(const string& str) const { return Gram_Fitness_Function::operator()(str, 2); }
    double Trigrams::operator()(const string& str) const { return Gram_Fitness_Function::operator()(str, 3); }
    double Quadgrams::operator()(const string& str) const { return Gram_Fitness_Function::operator()(str, 4); }
    
    
    
    unordered_map<string,double> initialize_gram(const int& len) {
        unordered_map<string,double> result = {};
        const vector<string> filepaths = {"english/letters.txt", "english/bigrams.txt", "english/trigrams.txt", "english/quadgrams.txt"};
        ifstream ifs(filepaths[len-1]);
        if (!ifs) {
            std::cerr << "Unable to locate english data.\n";
        } else {
            string line = "";
            while (!ifs.eof()) {
                getline(ifs,line);
                result[line.substr(0, len)] = std::stod(line.substr(len+1));
            }
            ifs.close();
        }
        return result; 
    }
    
} // namespace Denigma


