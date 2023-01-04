#pragma once

#include "variables/Root.h"


#include <filesystem>
#include <istream>

namespace omfl {

    Root ParseAllStatements(std::vector<std::string_view>& statements);
    Root parse(const std::filesystem::path& path);
    Root parse(const std::string& str);

}// namespace