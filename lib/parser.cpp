#include "parser.h"
#include "variables/SectionVariable.h"
#include "variables/StringVariable.h"
#include "variables/IntVariable.h"
#include "variables/ArrayVariable.h"
#include "variables/FloatVariable.h"
#include "variables/BoolVariable.h"

#include <fstream>
#include <charconv>


namespace omfl {
    bool IsCorrectKey(const std::string_view& key) {
        if (key.empty()) {
            return false;
        }
        return std::all_of(key.begin(),
                           key.end(),
                           [](char c) {
                               return isalnum(c) || c == '-' || c == '_';
                           });
    }

    void RemoveSpaces(std::string_view& str) {
        if (str.empty()) {
            return;
        }
        auto start = str.begin();
        auto end = str.end();
        for (; start != end && std::isspace(*start); ++start) {}
        if (start == end) {
            str = std::string_view(str.begin(), str.begin());
            return;
        }
        for (end -= 1; end != start && std::isspace(*end); --end) {}
        str = std::string_view(start, end + 1);
    }

    std::unique_ptr<IVariable> MakeSimpleVariable(std::string_view& source) {
        if (source.empty()) {
            throw std::exception();
        }
        if (source == "true") {
            return std::make_unique<BoolVariable>(BoolVariable(true));
        }
        if (source == "false") {
            return std::make_unique<BoolVariable>(BoolVariable(false));
        }
        if (source.starts_with('"')) {
            if (!source.ends_with('"') || source.length() < 2
                || std::find(source.begin() + 1, source.end() - 1, '"') != source.end() - 1) {
                throw std::exception();
            }
            return std::make_unique<StringVariable>(StringVariable(std::string(source.begin() + 1, source.end() - 1)));
        }
        bool positive = true;
        if (source.starts_with('+')) {
            source = source.substr(1);
        } else if (source.starts_with('-')) {
            source = source.substr(1);
            positive = false;
        }
        size_t dot_pos = source.find('.');
        if (dot_pos == std::string_view::npos) {
            if (source.empty() || !std::all_of(source.begin(), source.end(), isdigit)) {
                throw std::exception();
            }
            int absolute_value;
            std::from_chars(source.begin(), source.end(), absolute_value);
            if (positive) {
                return std::make_unique<IntVariable>(IntVariable(absolute_value));
            }
            return std::make_unique<IntVariable>(IntVariable(-absolute_value));
        }
        size_t dot_count = std::count(source.begin(), source.end(), '.');
        if (dot_count > 1) {
            throw std::exception();
        }
        std::string_view main_part(source.begin(), source.begin() + dot_pos);
        std::string_view float_part = source.substr(dot_pos + 1);
        if (main_part.empty() || float_part.empty()) {
            throw std::exception();
        }
        if (!std::all_of(main_part.begin(), main_part.end(), isdigit)) {
            throw std::exception();
        }
        if (!std::all_of(float_part.begin(), float_part.end(), isdigit)) {
            throw std::exception();
        }
        float absolute_value = std::strtof(std::string{source.begin(), source.end()}.c_str(), 0);
        if (positive) {
            return std::make_unique<FloatVariable>(FloatVariable(absolute_value));
        }
        return std::make_unique<FloatVariable>(FloatVariable(-absolute_value));
    }

    int FindArrayEnd(const std::string_view& s, int start) {
        int cur = 0;
        for (int i = start; i < s.length(); i++) {
            if (s[i] == '[') {
                ++cur;
            } else if (s[i] == ']') {
                --cur;
                if (cur == 0) {
                    return i;
                }
            }
        }
        return -1;
    }

    std::unique_ptr<ArrayVariable> MakeArray(const std::string_view& source) {
        std::vector<std::unique_ptr<IVariable>> array;
        int start = 0;
        for (int i = 0; i < source.length(); i++) {
            char c = source[i];
            if (c == '[') {
                int end = FindArrayEnd(source, i);
                if (end == -1) {
                    throw std::exception();
                }
                std::string_view value(source.begin() + i + 1, source.begin() + end);
                array.push_back(MakeArray(value));
                i = end + 1;
                start = end + 2;
            } else if (c == ',' || i == source.length() - 1) {
                std::string_view value;
                if (c != ',') {
                    value = std::string_view(source.begin() + start, source.end());
                } else {
                    value = std::string_view(source.begin() + start, source.begin() + i);
                }
                start = i + 1;
                RemoveSpaces(value);
                if (value.empty()) {
                    continue;
                }
                array.push_back(MakeSimpleVariable(value));
            }
        }
        return std::make_unique<ArrayVariable>(ArrayVariable(std::move(array)));
    }

    Root ParseAllStatements(std::vector<std::string_view>& statements) {
        Root root;
        SectionVariable* section_to_insert = &root;
        for (auto& statement : statements) {
            size_t sharp_pos = statement.find('#');
            if (sharp_pos != std::string_view::npos) {
                statement = std::string_view(statement.begin(), statement.begin() + sharp_pos);
            }
            RemoveSpaces(statement);
            if (statement.empty()) {
                continue;
            }

            if (statement.starts_with('[')) {
                if (!statement.ends_with(']') || statement.length() == 2) {
                    root.valid_ = false;
                    return root;
                }
                std::optional<SectionVariable*>
                    find_section = root.GetSection(std::string_view(statement.begin() + 1, statement.end() - 1));
                if (find_section == std::nullopt) {
                    root.valid_ = false;
                    return root;
                }
                section_to_insert = find_section.value();
            } else {
                size_t equal_sign_pos = statement.find('=');
                if (equal_sign_pos == std::string_view::npos) {
                    root.valid_ = false;
                    return root;
                }

                std::string_view key(statement.begin(), equal_sign_pos);
                std::string_view value(statement.begin() + equal_sign_pos + 1, statement.end());
                RemoveSpaces(key);
                RemoveSpaces(value);
                if (value.empty()) {
                    root.valid_ = false;
                    return root;
                }
                if (!IsCorrectKey(key)) {
                    root.valid_ = false;
                    return root;
                }
                if (value.starts_with('[')) {
                    if (!value.ends_with(']')) {
                        root.valid_ = false;
                        return root;
                    }
                    try {
                        bool good = section_to_insert->SetVariable(
                            key, MakeArray(std::string_view(value.begin() + 1, value.end() - 1))
                        );
                        if (!good) {
                            root.valid_ = false;
                            return root;
                        }
                    } catch (const std::exception& ex) {
                        root.valid_ = false;
                        return root;
                    }
                } else {
                    try {
                        if (!section_to_insert->SetVariable(key, MakeSimpleVariable(value))) {
                            root.valid_ = false;
                            return root;
                        }
                    } catch (const std::exception& ex) {
                        root.valid_ = false;
                        return root;
                    }
                }
            }
        }

        return root;
    }

    Root parse(const std::string& str) {
        std::vector<std::string_view> statements;
        size_t start = 0;
        for (int i = 0; i < str.length(); i++) {
            char c = str[i];
            if (c == '\n') {
                statements.emplace_back(str.begin() + start, str.begin() + i);
                start = i + 1;
            } else if (i == str.length() - 1) {
                statements.emplace_back(str.begin() + start, str.end());
            }
        }
        return ParseAllStatements(statements);
    }

    Root parse(const std::filesystem::path& path) {
        std::vector<std::string_view> statements;
        std::stringstream ss;
        std::ifstream fin(path);
        std::string line;

        while (std::getline(fin, line)) {
            ss << line << '\n';
        }
        return parse(ss.str());
    }
}
