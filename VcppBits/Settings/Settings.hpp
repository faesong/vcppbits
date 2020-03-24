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

#ifndef VcppBits__SETTINGS_HPP_INCLUDED__
#define VcppBits__SETTINGS_HPP_INCLUDED__

#include <map>
#include <string>

#include "VcppBits/Settings/Setting.hpp"

namespace VcppBits {

typedef std::map<std::string, Setting> SettingsMap;
typedef std::map<std::string, Setting*> SettingsPtrsMap;
typedef std::map<std::string, SettingsPtrsMap> SettingsCategories;

class SettingsIterator {
public:
    SettingsIterator (SettingsPtrsMap::iterator b,
                      SettingsPtrsMap::iterator e);

    void peekNext ();
    bool isElement () const;
    Setting& getCurrent () const;

private:
    const SettingsPtrsMap::iterator begin;
    const SettingsPtrsMap::iterator end;
    SettingsPtrsMap::iterator current;
    bool currentIsElement;
};

class SettingsCategoriesIterator {
public:
    SettingsCategoriesIterator (SettingsCategories::iterator b,
                                SettingsCategories::iterator e);
    void peekNext ();
    bool isElement () const;
    std::string getName ();
    SettingsIterator getSettingsIterator ();
private:
    const SettingsCategories::iterator begin;
    const SettingsCategories::iterator end;
    SettingsCategories::iterator current;
    bool currentIsElement;
};


// usage: 1) create instance with (optional) filename
// 2) add settings by calling .appendSetting
// call .load() to load setting values from file
class Settings {
public:
    explicit Settings (const std::string& filename = "");
    ~Settings ();
    Setting& appendSetting (Setting set);
    void load ();
    void setFilename (const std::string& name) {
        this->filename = name;
    }
    template<typename T> T get (const std::string& name) const {
        const T fake{};
        return getSetting(name).getValue(fake);
    }
    void loadFromFile (const std::string &pFileName);
    void reloadFromFile ();
    const Setting& getSetting (const std::string& name) const;
    Setting& getSetting (const std::string& name);
    bool hasSetting (const std::string& name) const;
    SettingsIterator getSettingsIterator ();
    SettingsIterator  getCategorySettingsIterator (const std::string& cat_name);
    SettingsCategoriesIterator getSettingsCategoriesIterator ();
    void writeFile ();
private:

    void loadSettings ();

    SettingsMap values;
    SettingsPtrsMap valuesMap;
    SettingsCategories categories;
    std::string filename;
};

} // namespace VcppBits

#endif // VcppBits__SETTINGS_HPP_INCLUDED__
