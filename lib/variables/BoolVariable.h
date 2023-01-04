#pragma once

#include "IVariable.h"

class BoolVariable : public IVariable {
 private:
    std::string name_;
    bool value_;
 public:
    explicit BoolVariable(bool value, std::string name = "") :
        value_(value), name_(std::move(name)) {}

    bool AsBool() const override {
        return value_;
    }

    bool AsBoolOrDefault(bool default_bool) const override {
        return value_;
    }

    bool IsBool() const override {
        return true;
    }
};