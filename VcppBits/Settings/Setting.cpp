// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

// Copyright 2015-2020 Vitalii Minnakhmetov <restlessmonkey@ya.ru>
//
// This file is part of CppBits.
//
// CppBits is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CppBits Top is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppBits.  If not, see <http://www.gnu.org/licenses/>.


#include "Setting.hpp"
#include <cassert>
#include <stdexcept>
#include <algorithm>

#include "SettingsException.hpp"

namespace VcppBits {


Setting::Setting (const std::string &pName)
    : type (S_INTEGER),
      valuetype (INTEGER),
      name (pName),
      _value (false),
      _defaultValue (false),
      _upperValue (false),
      _lowerValue (false) {
    throw std::runtime_error("Setting::Setting(std::string) should never be called");
    // should never be called; added for std::map
}
Setting::Setting (const std::string &pName, const bool default_val)
    : type (S_BOOL),
      valuetype (BOOLEAN),
      name (pName),
      _value (default_val),
      _defaultValue (default_val) {
}
Setting::Setting (const std::string &pName, const int default_val)
    : type (S_INTEGER),
      valuetype (INTEGER),
      name (pName),
      _value (default_val),
      _defaultValue (default_val) {
}
Setting::Setting (const std::string &pName,
                  const int down_bound,
                  const int up_bound,
                  const int default_val)
    : type (S_INTEGER_BOUNDED),
      valuetype (INTEGER),
      name (pName),
      _value (default_val),
      _defaultValue (default_val),
      _upperValue (up_bound),
      _lowerValue (down_bound) {
    assert (default_val >= down_bound);
    assert (default_val <= up_bound);
}
Setting::Setting (const std::string &pName,
                  const float default_val)
    : type (S_FLOATINGPOINT),
      valuetype (FLOATINGPOINT),
      name (pName),
      _value (default_val),
      _defaultValue (default_val) {
}
Setting::Setting (const std::string &pName,
                  const float down_bound,
                  const float up_bound,
                  const float default_val)
    : type (S_FLOATINGPOINT_BOUNDED),
      valuetype (FLOATINGPOINT),
      name (pName),
      _value (default_val),
      _defaultValue (default_val),
      _upperValue (up_bound),
      _lowerValue (down_bound) {
    assert (default_val >= down_bound);
    assert (default_val <= up_bound);
}
Setting::Setting (const std::string &pName,
                  const std::string &default_val)
    : type (S_STRING),
      valuetype (STRING),
      name (pName),
      stringVal (default_val),
      stringDefaultVal (default_val) {
}
Setting::Setting (const std::string &pName,
                  const std::vector<std::string> &possible_vals,
                  const std::string &default_val)
    : type (S_STRING_ONE_OF),
      valuetype (STRING),
      name (pName),
      stringDefaultVal (default_val),
      stringPossibleVals (possible_vals) {
    setString(default_val);
}

Setting& Setting::setDescription (const std::string &descr) {
    this->description = descr;
    return *this;
}

std::string Setting::getDescription () const {
    return this->description;
}

Setting& Setting::setLongName (const std::string &long_name) {
    this->longName = long_name;
    return *this;
}

std::string Setting::getLongName () const {
    return this->longName;
}

Setting::TYPE Setting::getType () const {
    return this->type;
}
Setting::VALUETYPE Setting::getValueType () const {
    return this->valuetype;
}

std::string Setting::getName () const {
    return this->name;
}
bool Setting::getBool () const {
    assert (this->valuetype == BOOLEAN);
    return _value.boolVal;
}
bool* Setting::getBoolPtr () {
    assert (this->valuetype == BOOLEAN);
    return &(_value.boolVal);
}
bool Setting::getBoolDefault () const {
    assert (this->valuetype == BOOLEAN);
    return _defaultValue.boolVal;
}
int Setting::getInt () const {
    assert (this->valuetype == INTEGER);
    return _value.intVal;
}

int* Setting::getIntPtr () {
    assert (this->valuetype == INTEGER);
    return &(_value.intVal);
}

int Setting::getIntDefault () const {
    assert (this->valuetype == INTEGER);
    return _defaultValue.intVal;
}
int Setting::getIntDown () const {
    assert (this->valuetype == INTEGER);
    return _lowerValue.intVal;
}
int Setting::getIntUp () const {
    assert (this->valuetype == INTEGER);
    return _upperValue.intVal;
}
float Setting::getFloat () const {
    assert (this->valuetype == FLOATINGPOINT);
    return _value.floatVal;
}
float* Setting::getFloatPtr () {
    assert (this->valuetype == FLOATINGPOINT);
    return &(_value.floatVal);
}
float Setting::getFloatDefault () const {
    assert (this->valuetype == FLOATINGPOINT);
    return _defaultValue.floatVal;
}
float Setting::getFloatDown () const {
    assert (this->valuetype == FLOATINGPOINT);
    return _lowerValue.floatVal;
}
float Setting::getFloatUp () const {
    assert (this->valuetype == FLOATINGPOINT);
    return _upperValue.floatVal;
}
std::string Setting::getString () const {
    assert (this->valuetype == STRING);
    return this->stringVal;
}

std::iterator_traits<std::vector<std::string>::iterator>::difference_type
Setting::getStringPos () const {
    assert (this->type == S_STRING_ONE_OF);
    return this->stringPos;
}

std::string Setting::getDefaultString () const {
    assert (this->valuetype == STRING);
    return this->stringDefaultVal;
}
std::vector<std::string> Setting::getPossibleStrings() const {
    assert (this->valuetype == STRING);
    return this->stringPossibleVals;
}
void Setting::setBool (const bool new_val) {
    assert(this->valuetype == BOOLEAN);
    _value.boolVal = new_val;
    this->onChangeEvent();
}
void Setting::setInt (const int new_val) {
    assert (this->valuetype == INTEGER);
    if (this->type == S_INTEGER_BOUNDED
        && (new_val < _lowerValue.intVal || new_val > _upperValue.intVal)) {
        throw SettingsException(this->name, SettingsException::OUT_OF_RANGE);
    }
    _value.intVal = new_val;
    this->onChangeEvent();
}
void Setting::setFloat (const float new_val) {
    assert (this->valuetype == FLOATINGPOINT);
    if (this->type == S_FLOATINGPOINT_BOUNDED
        && (new_val < _lowerValue.floatVal || new_val > _upperValue.floatVal)) {
        throw SettingsException(this->name, SettingsException::OUT_OF_RANGE);
    }
    _value.floatVal = new_val;
    this->onChangeEvent();
}
void Setting::setString (const std::string &new_val) {
    assert (this->valuetype == STRING);
    auto pos = std::find(stringPossibleVals.begin(),
                         stringPossibleVals.end(),
                         new_val);
    if (this->type == S_STRING_ONE_OF) {
        if (pos == stringPossibleVals.end()) {
            throw SettingsException(this->name, SettingsException::OUT_OF_RANGE);
        }
        this->stringPos = std::distance(stringPossibleVals.begin(), pos);
    }
    this->stringVal = new_val;
    this->onChangeEvent();
}
void Setting::setByString (const std::string &new_val) {
    using SettingsStringUtils::fromString;
    switch (this->valuetype) {
    case Setting::BOOLEAN:
        this->setBool(fromString<int>(new_val));
        break;
    case Setting::INTEGER:
        this->setInt(fromString<int>(new_val));
        break;
    case Setting::FLOATINGPOINT:
        this->setFloat(fromString<float>(new_val));
        break;
    case Setting::STRING:
        this->setString(new_val);
        break;
    }
}
std::string Setting::getAsString () const {
    using SettingsStringUtils::toString;
    switch (this->valuetype) {
    case Setting::BOOLEAN:
        return toString<int>(_value.boolVal);
        break;
    case Setting::INTEGER:
        return toString<int>(_value.intVal);
        break;
    case Setting::FLOATINGPOINT:
        return toString<float>(_value.floatVal);
        break;
    case Setting::STRING:
        return this->stringVal;
        break;
    }
    return "";
}

void Setting::removeUpdateHandler (const void *listener_id) {
    std::vector<SettingListener>::iterator
        it = this->listeners.begin(),
        end = this->listeners.end();
    for (; it != end; ++it) {
        if (it->first == listener_id) {
            this->listeners.erase(it);
            return;
        }
    }
}

void Setting::addUpdateHandler (void *listener_id,
                                std::function<void()> handler) {
    listeners.push_back(std::make_pair(listener_id, handler));
}


void Setting::onChangeEvent () {
    std::vector<SettingListener>::iterator
        it = this->listeners.begin(),
        end = this->listeners.end();
    int i = 0;
    for (; it != end; ++it) {
        it->second();
        ++i;
    }

    // now call the callbaks
}

} // namespace VcppBits
