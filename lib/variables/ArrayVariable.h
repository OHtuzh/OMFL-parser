#pragma once

#include "IVariable.h"

#include <vector>

class ArrayVariable : public IVariable {
 private:
    std::string name_;
    std::vector<std::shared_ptr<IVariable>> value_;
 public:
    explicit ArrayVariable(std::vector<std::shared_ptr<IVariable>> value, std::string name = "") :
        value_(std::move(value)), name_(std::move(name)) {}

    const IVariable& operator[](size_t index) const override {
        if (index >= value_.size()) {
            return *(new IVariable());
        }
        return *value_[index];
    }

    bool IsArray() const override {
        return true;
    }
};
