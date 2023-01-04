#pragma once

#include "IVariable.h"

class FloatVariable : public IVariable {
 private:
    std::string name_;
    float value_;
 public:
    explicit FloatVariable(float value, std::string name = "") :
        value_(value), name_(std::move(name)) {}

    float AsFloat() const override {
        return value_;
    }

    float AsFloatOrDefault(float default_float) const override {
        return value_;
    }

    bool IsFloat() const override {
        return true;
    }
};
