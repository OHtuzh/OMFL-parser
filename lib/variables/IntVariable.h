#pragma once

#include "IVariable.h"

class IntVariable : public IVariable {
 private:
    std::string name_;
    int value_;

 public:
    explicit IntVariable(int value, std::string name = "") :
        name_(std::move(name)), value_(value) {}

    int AsInt() const override {
        return value_;
    }

    int AsIntOrDefault(int default_int) const override {
        return value_;
    }

    bool IsInt() const override {
        return true;
    }
};