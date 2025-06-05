// A library to help catalog command line flags and inputs.

#include<vector>
#include<string>
#include<map>
#include<algorithm>

namespace cli_interface {
    
    using std::vector; 
    using std::string; 
    using std::map; 
    
    bool yes_no(const char& c) { return !(c == 'n' || c == 'N' || c == '0'); }
    
    // Given the CLI arguments, match single-digit flags to the other arguments
    map<char, string> get_arguments(const vector<string>& vec) {
        
        vector<char> flags = {};
        vector<string> values = {};
        
        // All arguments starting '-' will be treated as accumulations of flags.
        for (int i = 1; i < vec.size(); ++i) // Skip the program name. 
            if (vec[i][0] == '-')   
                for (int j = 1; j < vec[i].size(); ++j) // Skip the '-' itself.
                    flags.push_back(vec[i][j]);
            else 
                values.push_back(vec[i]);
        
        // Add placeholders to the start of the values vector so sizes match.
        // Intended so that leading flags can later be treated as switches.
        while (values.size() < flags.size())  
            values.insert(values.begin(), "");
        
        // Put the two vectors together.
        map<char, string> result = {}; 
        for (int i = 0; i < flags.size(); ++i) 
            result.insert({flags[i], values[i]});
        return result; 
    }
    map<char, string> get_arguments(const int& argc, const char* const argv[]) { return get_arguments(vector<string>{argv, argv + argc}); }
    
    
    map<string,vector<string>> get_block_arguments(const vector<string>& args, vector<string> flags) {
        map<string,vector<string>> result = {}; 
        
        string flag = "";
        vector<string> block_argument = {};
        auto flag_iter = flags.begin();
        for (const string& arg : args) {
            flag_iter = std::find(flags.begin(), flags.end(), arg);
            if (flag_iter == flags.end()) {
                block_argument.push_back(arg);
            } else {
                result[flag] = block_argument; 
                flag = *flag_iter;
                block_argument = {};
            }
        }
        result[flag] = block_argument; 
        
        /*for (auto block : result)
            if (block.second.empty())
                result.erase(block.first);*/
        
        return result; 
    }
    map<string,vector<string>> get_block_arguments(const int& argc, const char* const argv[], vector<string> flags) { return get_block_arguments(vector<string>{argv, argv + argc}, flags); }
    
    
    
} // cli_interface

