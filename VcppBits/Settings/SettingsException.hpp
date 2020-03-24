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


#ifndef VcppBits_SETTINGS_EXCEPTION_HPP_INCLUDED__
#define VcppBits_SETTINGS_EXCEPTION_HPP_INCLUDED__

#include <string>
#include <stdexcept>

#include "VcppBits/Settings/SettingsStringUtils.hpp"

namespace VcppBits {

class SettingsException : std::exception {
public:
    enum TYPE { ALREADY_LOADED, NOT_FOUND, OUT_OF_RANGE };
    SettingsException (const std::string &setting_name, TYPE t)
        : settingName(setting_name),
          type (t) {
    }
    ~SettingsException () throw () {}

    const std::string getFullDescription () const {
        return std::string("Setting Error ")
            + SettingsStringUtils::toString(this->type) + ": \""
            + settingName + "\"";
    }

    const char *what () const throw() {
        return this->getFullDescription().c_str();
    }

    const std::string settingName;
    const TYPE type;
};

} // namespace VcppBits

#endif // VcppBits_SETTINGS_EXCEPTION_HPP_INCLUDED__
