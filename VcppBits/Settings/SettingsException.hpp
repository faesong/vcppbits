// The MIT License (MIT)

// Copyright 2015-2020 Vitalii Minnakhmetov <restlessmonkey@ya.ru>

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.


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

    const std::string& getFullDescription () const {
        _errorMessage = std::string("Setting Error ")
            + SettingsStringUtils::toString(this->type) + ": \""
            + settingName + "\"";
        return _errorMessage;
    }

    const char *what () const throw() {
        return this->getFullDescription().c_str();
    }

    const std::string settingName;
    mutable std::string _errorMessage;
    const TYPE type;
};

} // namespace VcppBits

#endif // VcppBits_SETTINGS_EXCEPTION_HPP_INCLUDED__
