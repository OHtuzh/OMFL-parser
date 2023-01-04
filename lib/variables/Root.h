#pragma once

#include "SectionVariable.h"
#include "vector"
#include "../parser.h"

namespace omfl {
    class Root : public SectionVariable {
     private:
        friend Root ParseAllStatements(std::vector<std::string_view>& statements);
        bool valid_;
     public:
        Root() : SectionVariable(""), valid_(true) {}

        bool valid() const {
            return valid_;
        }
    };
}
