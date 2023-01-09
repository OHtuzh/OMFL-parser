#pragma once

#include "IVariable.h"
#include "Root.h"

#include <unordered_map>
#include <optional>
#include <sstream>
#include "memory"

class SectionVariable : public IVariable {
 private:
    struct CustomStringHasher {
        using is_transparent = void;

        std::size_t operator()(const std::string& s) const {
            return std::hash<std::string>()(s);
        }

        std::size_t operator()(const std::string_view& sv) const {
            return std::hash<std::string_view>()(sv);
        }
    };
    std::string name_;
    std::unordered_map<std::string, std::shared_ptr<IVariable>, CustomStringHasher, std::equal_to<>> value_;

 public:
    explicit SectionVariable(std::string name) :
        IVariable(),
        name_(std::move(name)) {}

    bool SetVariable(const std::string_view& key, const std::shared_ptr<IVariable>& value) {
        if (value_.find(key) != value_.end()) {
            return false;
        }
        value_.insert(std::make_pair(key, value));
        return true;
    }

    std::optional<SectionVariable*> GetSection(const std::string_view& section) {
        if (section.empty() || section.back() == '.') {
            return std::nullopt;
        }
        std::stringstream current_section;
        SectionVariable* current_section_pointer = this;
        for (int i = 0; i < section.size(); i++) {
            char c = section[i];
            if (i == section.size() - 1) {
                current_section << c;
            }
            if (c == '.' || i == section.size() - 1) {
                std::string section_name = current_section.str();
                if (section_name.empty()) {
                    return std::nullopt;
                }
                if (current_section_pointer->value_.find(section_name) != current_section_pointer->value_.end()) {
                    auto variable = current_section_pointer->value_.at(section_name);
                    current_section_pointer = reinterpret_cast<SectionVariable*>(variable.get());
                } else {
                    current_section_pointer->value_[section_name] = std::make_shared<SectionVariable>(SectionVariable(section_name));
                    current_section_pointer =
                        reinterpret_cast<SectionVariable*>(current_section_pointer->value_.at(section_name).get());
                }
                current_section = std::stringstream("");
            } else {
                current_section << c;
            }
        }
        return current_section_pointer;
    }

    const IVariable& Get(const std::string& path) const override {
        size_t last_dot = path.find_last_of('.');
        if (last_dot == std::string::npos) {
            return *value_.at(path);
        }
        std::string_view section_path(path.begin(), path.begin() + last_dot);
        std::string_view variable_name(path.begin() + last_dot + 1, path.end());
        std::stringstream current_section;
        const SectionVariable* section = this;
        for (int i = 0; i < section_path.size(); i++) {
            char c = section_path[i];
            if (i == section_path.size() - 1) {
                current_section << c;
            }
            if (c == '.' || i == section_path.size() - 1) {
                std::string cur_sec = current_section.str();
                section = reinterpret_cast<SectionVariable*>(section->value_.at(current_section.str()).get());
                current_section = std::stringstream("");
            } else {
                current_section << c;
            }
        }
        return *section->value_.at(std::string(variable_name));
    }

    bool IsSection() const override {
        return true;
    }
};