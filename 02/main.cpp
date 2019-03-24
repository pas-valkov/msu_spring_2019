#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

using namespace std;

vector<string> split(const string& strToSplit) {
    //~ vector<char> delimiters = {'+', '-', '*', '/'};
    vector<string> splittedStrings;
    for (int64_t i = 0; i < strToSplit.size(); ++i) {
        char c = strToSplit[i];
        if (c == ' ')
            ;
        else if (c == '+' or c == '-' or c == '*' or c == '/') {
            splittedStrings.push_back(string(1, c));
        }
        else if (c <= '9' and c >= '0') {
            int64_t leng = 0;
            while (c <= '9' and c >= '0' and i < strToSplit.size()) {
                leng++; c = strToSplit[++i];
            }
            splittedStrings.push_back(strToSplit.substr(i-leng, leng));
            --i;
        }
        else {
            throw string("error");
        }
    }
    return splittedStrings;
}

int64_t get_int(const vector<string>& args, int64_t& id) {
    int64_t number = 0;
    bool is_negative = false;
    while (id < args.size()) {
        if (args[id] == "+")
            ;
        else if (args[id] == "-")
            is_negative = !is_negative;
        else {
            number = atoi(args[id++].c_str());
            return is_negative == false ? number : -number;
        }
        ++id;
    }
    return number;
}

bool no_consecutive_numbers(const vector<string>& s) {
    bool is_op_prev = (s[0] == "+" or s[0] == "-" 
                    or s[0] == "*" or s[0] == "/");
    for (int64_t i = 1; i < s.size(); ++i) {
        bool is_op = (s[1] == "+" or s[1] == "-" 
                   or s[1] == "*" or s[1] == "/");
        if (!is_op and !is_op_prev) {
            throw string("error");
        }
        is_op_prev = is_op;
    }
    return true;
}

int64_t calc_term(const vector<string>& args, int64_t first, int64_t& id) {
    int64_t term_mul = first;
    while (id < args.size()) {
        char op = args[id][0];
        
        if (op != '*' and op != '/') {
            return term_mul;
        }
        int64_t cur_int = get_int(args, ++id);
        if (op == '*') {
            term_mul *= cur_int;
        }
        else if (op == '/') {
            if (cur_int == 0) {
                throw string("error");
            }
            term_mul /= cur_int;
        }
        else {
            throw string("error");
        }
    }
    return term_mul;
}

int64_t calc_sum(const vector<string>& args) {
    int64_t i = 0;
    int64_t sum = 0;
    while (i < args.size()) {
        int64_t cur_int = get_int(args, i);
        if (i == args.size())
            return sum + cur_int;
        else {
            if (args[i] == "*" or args[i] == "/") {
                sum += calc_term(args, cur_int, i);
            }
            else {
                sum += cur_int;
            }
        }
    }
    return sum;
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    if (argc != 2) {
        cout << "error" << endl;
        return 1;
    }
    try {
        vector<string> splited = split(argv[1]);
        if (splited.back() > "9" or splited.back() < "0")
            throw string("error");
        cout << calc_sum(splited) << endl;
    }
    catch (string& error_str) {
        cout << error_str << endl;
        return 1;
    }
    return 0;
}
