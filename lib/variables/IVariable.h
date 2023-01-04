#pragma once

#include <string>
#include <stdexcept>
#include <optional>

class IVariable {
 public:
    virtual const IVariable& Get(const std::string& section) const {
        throw std::bad_cast();
    }

    virtual const IVariable& operator[](size_t index) const {
        throw std::bad_cast();
    }

    virtual bool AsBool() const {
        throw std::bad_cast();
    }

    virtual int AsInt() const {
        throw std::bad_cast();
    }

    virtual float AsFloat() const {
        throw std::bad_cast();
    }

    virtual std::string AsString() const {
        throw std::bad_cast();
    }

    virtual bool AsBoolOrDefault(bool default_bool) const {
        return default_bool;
    }

    virtual int AsIntOrDefault(int default_int) const {
        return default_int;
    }

    virtual float AsFloatOrDefault(float default_float) const {
        return default_float;
    }

    virtual std::string AsStringOrDefault(const std::string& default_string) const {
        return default_string;
    }

    virtual bool IsBool() const {
        return false;
    }

    virtual bool IsInt() const {
        return false;
    }

    virtual bool IsFloat() const {
        return false;
    }

    virtual bool IsString() const {
        return false;
    }

    virtual bool IsArray() const {
        return false;
    }

    virtual bool IsSection() const {
        return false;
    }
};

