// Copyright 2015-2020 Vitalii Minnakhmetov <restlessmonkey@ya.ru>
//
// This file is part of CppBits.
//
// CppBits is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CppBits is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppBits.  If not, see <http://www.gnu.org/licenses/>.

#ifndef VcppBits_SETTINGS_STRING_UTILS_HPP_INCLUDED__
#define VcppBits_SETTINGS_STRING_UTILS_HPP_INCLUDED__


#include <string>
#include <sstream>

namespace VcppBits {

namespace SettingsStringUtils { // pretty commonly named functions are there...

template <typename T>
inline T fromString (const std::string &str) {
    std::stringstream ss_val(str);
    T ret_val;
    ss_val >> ret_val;
    return ret_val;
}

template <typename T>
inline std::string toString (const T &val) {
    std::stringstream ss_val;
    ss_val << val;
    return ss_val.str();
}
} // namespace SettingsStringUtils
} // namespace VcppBits

#endif // VcppBits_SETTINGS_STRING_UTILS_HPP_INCLUDED__
