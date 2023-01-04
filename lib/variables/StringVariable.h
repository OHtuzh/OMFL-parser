#pragma once

#include "IVariable.h"

class StringVariable : public IVariable {
 private:
    std::string value_;
 public:
    explicit StringVariable(std::string value, std::string name = "") :
        value_(std::move(value)) {}

    std::string AsString() const override {
        return value_;
    }

    std::string AsStringOrDefault(const std::string& default_string) const override {
        return value_;
    }

    bool IsString() const override {
        return true;
    }
};
