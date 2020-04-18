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
