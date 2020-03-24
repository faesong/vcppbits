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


#include "VcppBits/Settings/Settings.hpp"
#include <vector>

#include "VcppBits/KeyFile/KeyFile.hpp"

#include "VcppBits/Settings/SettingsException.hpp"

namespace VcppBits {

SettingsIterator::SettingsIterator (SettingsPtrsMap::iterator b,
                                    SettingsPtrsMap::iterator e)
    : begin (b),
      end (e),
      current (b),
      currentIsElement (current != end) {
}

void SettingsIterator::peekNext () {
    if (this->currentIsElement) {
        ++this->current;

        if (this->current == this->end) {
            this->currentIsElement = false;
        }
    }
}

bool SettingsIterator::isElement () const {
    return this->currentIsElement;
}

Setting& SettingsIterator::getCurrent () const {
    return *(*current).second;
}



SettingsCategoriesIterator::SettingsCategoriesIterator
    (SettingsCategories::iterator b,
     SettingsCategories::iterator e)
    : begin (b),
      end (e),
      current (b),
      currentIsElement (current != end)
{
}

void SettingsCategoriesIterator::peekNext () {
    if (this->currentIsElement) {
        ++this->current;

        if (this->current == this->end) {
            this->currentIsElement = false;
        }
    }
}

bool SettingsCategoriesIterator::isElement () const {
    return this->currentIsElement;
}

std::string SettingsCategoriesIterator::getName () {
    return (*current).first;
}

SettingsIterator SettingsCategoriesIterator::getSettingsIterator () {
    return SettingsIterator ((*current).second.begin(), (*current).second.end());
}

Settings::Settings (const std::string &pFilename/* = "" */)
    : filename (pFilename) {
}

void Settings::load () {
    if (!this->filename.size()) {
        return;
    }
    try {
        // TODO10: migrate to range-based for
        KeyFile f (this->filename);
        KeyFileSectionsIterator sec_iter = f.getSectionsIterator();
        while (sec_iter.isElement()) {
            KeyFileSettingsIterator set_iter = sec_iter.getSettingsIterator();
            const std::string sec_name = sec_iter.getName();
            const std::string prefix =
                sec_name.empty() ? "" : sec_name + ".";
            while (set_iter.isElement()) {
                if (this->values.count(prefix + set_iter.getName())) {
                    try {
                        this->values[prefix + set_iter.getName()]
                            .setByString(set_iter.getValue());
                    }
                    catch (const SettingsException& oor) {
                      (void) oor;
                    }
                }
                set_iter.peekNext();
            }
            sec_iter.peekNext();
        }
    }
    catch (const KeyFile::file_not_found&) {
    }
}

void Settings::reloadFromFile () {
    this->load();
}

void Settings::loadFromFile (const std::string &name) {
    this->filename = name;
    this->load();
}

Settings::~Settings () {
    writeFile();
}

void Settings::writeFile () {
    if (!this->filename.size()) {
        return;
    }
    KeyFile file;
    SettingsMap::const_iterator
        it = this->values.begin(),
        end = this->values.end();
    for (;it != end; ++it) {
        file.appendKey((*it).first, (*it).second.getAsString());
    }
    file.writeToFile(this->filename);
}

// TODO3: cleanup empty??
void Settings::loadSettings () {

}

const Setting& Settings::getSetting (const std::string& name) const {
    if (this->valuesMap.count(name) < 1) {
        throw SettingsException(name, SettingsException::NOT_FOUND);
    }

    return *valuesMap.at(name);
}

Setting& Settings::getSetting (const std::string& name) {
    if (this->valuesMap.count(name) < 1) {
        throw SettingsException(name, SettingsException::NOT_FOUND);
    }

    return *valuesMap.at(name);
}


bool Settings::hasSetting (const std::string& name) const {
    return this->valuesMap.count(name);
}

SettingsIterator Settings::getSettingsIterator () {
    return SettingsIterator (this->valuesMap.begin(),
                             this->valuesMap.end());
}

SettingsIterator
Settings::getCategorySettingsIterator (const std::string& cat_name) {
    return SettingsIterator(this->categories[cat_name].begin(),
                            this->categories[cat_name].end());
}

SettingsCategoriesIterator Settings::getSettingsCategoriesIterator () {
    return SettingsCategoriesIterator (this->categories.begin(),
                                       this->categories.end());
}
Setting& Settings::appendSetting (Setting set) {
    if (this->valuesMap.count(set.getName()) > 0) {
        throw SettingsException(set.getName(),
                                SettingsException::ALREADY_LOADED);
    }

    this->values.insert(SettingsMap::value_type(set.getName(), set));
    Setting& inserted_set = this->values[set.getName()];
    this->valuesMap.insert(SettingsPtrsMap::value_type(set.getName(),
                                                       &inserted_set));

    size_t n = set.getName().find('.', 0);
    std::string categ_name =
        n == std::string::npos ? "" : set.getName().substr(0, n);

    this->categories[categ_name][set.getName()] = &inserted_set;

    return inserted_set;
}

} // namespace VcppBits
