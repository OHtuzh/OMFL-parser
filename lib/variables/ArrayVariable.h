#pragma once

#include "IVariable.h"

#include <vector>

class ArrayVariable : public IVariable {
 private:
    std::string name_;
    std::vector<std::unique_ptr<IVariable>> value_;
    constexpr const static IVariable empty_variable_{};
 public:
    explicit ArrayVariable(std::vector<std::unique_ptr<IVariable>> value, std::string name = "") :
        value_(std::move(value)), name_(std::move(name)) {}

    [[nodiscard]] const IVariable& operator[](size_t index) const override {
        if (index >= value_.size()) {
            return empty_variable_;
        }
        return *value_[index];
    }

    [[nodiscard]] bool IsArray() const override {
        return true;
    }
};
