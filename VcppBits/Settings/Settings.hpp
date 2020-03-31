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
    std::string getName () const;
    SettingsIterator getSettingsIterator ();
private:
    const SettingsCategories::iterator begin;
    const SettingsCategories::iterator end;
    SettingsCategories::iterator current;
    bool currentIsElement;
};

class SettingsSectionView {
public:
    class iterator {
    public:
        explicit iterator (const SettingsPtrsMap::iterator &pIt)
            : _current (pIt) {
        }
        iterator operator++() {
            ++_current;
            return *this;
        }
        bool operator!=(const iterator& pOther) const {
            return _current != pOther._current;
        }
        Setting& operator*() const {
            return *(_current->second);
        }
    private:
        SettingsPtrsMap::iterator _current;
    };

    iterator begin () { return iterator(_settingsPtrsMap.begin()); }
    iterator end () { return iterator(_settingsPtrsMap.end()); }

    SettingsSectionView (const std::string& pName,
                         SettingsPtrsMap& pSettingsPtrsMap)
        : _name (pName),
          _settingsPtrsMap (pSettingsPtrsMap) {
    }

    const std::string& getName() const {
        return _name;
    }

private:

    const std::string& _name;
    SettingsPtrsMap& _settingsPtrsMap;
};

// usage: 1) create instance with (optional) filename
// 2) add settings by calling .appendSetting
// call .load() to load setting values from file
class Settings {
public:
    class iterator {
    public:
        iterator (const SettingsCategories::iterator &pIt)
            : _current (pIt) {
        }
        iterator operator++() {
            ++_current;
            return *this;
        }
        bool operator!=(const iterator& pOther) const {
            return _current != pOther._current;
        }
        SettingsSectionView operator*() const {
            return SettingsSectionView(_current->first, _current->second);
        }
    private:
        SettingsCategories::iterator _current;
    };

    iterator begin () { return iterator(this->categories.begin()); }
    iterator end () { return iterator(this->categories.end()); }

    explicit Settings (const std::string& filename = "");
    ~Settings ();
    Setting& appendSetting (Setting set);
    void load ();
    void resetAll ();
    void setFilename (const std::string& name) {
        this->filename = name;
    }
    template<typename T> T get (const std::string& name) const {
        return getSetting(name).getValue<T>();
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
