#pragma once

#include <algorithm>

enum class Error {
    NoError,
    CorruptedArchive
};

class Serializer {
    static constexpr char Separator = ' ';
    std::ostream& out_;
    
public:
    explicit Serializer(std::ostream& out)
        : out_(out) {
    }

    template <class T>
    Error save(T& object) {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT... args) {
        return process(args...);
    }
    
private:
    template <class T, class... Args>
    Error process(T&& val, Args&&... args) {
        Error err = Error::CorruptedArchive;
        process(val);
        process(std::forward<Args>(args)...);
        return err = Error::NoError;
    }
    
    void process(bool& arg) {
        out_ << (arg ? "true" : "false") << Separator;
    }
    void process(uint64_t& arg) {
        out_ << arg << Separator;
    }
};


class Deserializer {
    std::istream& in_;
    
public:
    explicit Deserializer(std::istream& in)
        : in_(in) {
    }

    template <class T>
    Error load(T& object) {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&... args) {    //    &&?
        return process(args...);
    }
    
private:
    template <class T, class... Args>
    Error process(T&& val, Args&&... args) {
        if (process(val) == Error::NoError)
            return process(std::forward<Args>(args)...);
        else
            return Error::CorruptedArchive;
    }
    
    Error process(bool& arg) {
        std::string s;
        in_ >> s;
        if (s == "true")
            arg = true;
        else if (s == "false")
            arg = false;
        else
            return Error::CorruptedArchive;
        return Error::NoError;
    }
    
    Error process(uint64_t& arg) {
        std::string s;
        in_ >> s;
        if(is_uint64_t_number(s)) {
            std::istringstream iss(s);
            iss >> arg;
            return Error::NoError;
        }
        else
            return Error::CorruptedArchive;
    }
    
    bool is_uint64_t_number(const std::string& s) {
        return !s.empty() && std::find_if(s.begin(), 
            s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
    } 
};
