// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


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


#include "VcppBits/KeyFile/KeyFile.hpp"

#include <cassert>
#include <fstream>

#include "VcppBits/StringUtils/StringUtils.hpp"

namespace VcppBits {

KeyFileSettingsIterator::KeyFileSettingsIterator (KeyFileSettings::iterator b,
                                                  KeyFileSettings::iterator e,
                                                  KeyFileSettings &settings)
    : mBegin (b),
      mEnd (e),
      mCurrent (b),
      mSettings (settings),
      mCurrentIsElement (!(mCurrent == mEnd)) {
}


void KeyFileSettingsIterator::peekNext () {
    if (mCurrentIsElement) {
        mCurrent++;

        if (mCurrent == mEnd) {
            mCurrentIsElement = false;
        }
    }
    else {
        throw KeyFileOutOfRangeException();
    }
}


bool KeyFileSettingsIterator::isElement () const {
    return mCurrentIsElement;
}


KeyFileSettings::value_type KeyFileSettingsIterator::getSetting () const {
    if (!mCurrentIsElement) {
        throw KeyFileOutOfRangeException();
    }

    return *mCurrent;
}


std::string KeyFileSettingsIterator::getName () const {
    if (!mCurrentIsElement) {
        throw KeyFileOutOfRangeException();
    }

    return mCurrent->first;
}


std::string KeyFileSettingsIterator::getValue () const {
    if (!mCurrentIsElement) {
        throw KeyFileOutOfRangeException();
    }
    return mCurrent->second;
}


std::string KeyFileSettingsIterator::findSetting (const std::string &name) const {
    if (!mSettings.count(name)) {
        throw KeyFileSettingNotFoundException();
    }

    return mSettings[name];
}




KeyFileSectionsIterator::KeyFileSectionsIterator (KeyFileSections::const_iterator b,
                                      KeyFileSections::const_iterator e)
    : mBegin(b),
      mEnd(e),
      mCurrent(b) {
    mCurrentIsElement = !(mCurrent == mEnd);
}


void KeyFileSectionsIterator::peekNext () {
    if (mCurrentIsElement) {
        ++mCurrent;

        if (mCurrent == mEnd) {
            mCurrentIsElement = false;
        }
    }
    else {
        throw KeyFileOutOfRangeException();
    }
}


bool KeyFileSectionsIterator::isElement () const {
    return mCurrentIsElement;
}


std::string KeyFileSectionsIterator::getName () const {
    if (!mCurrentIsElement) {
        throw KeyFileOutOfRangeException();
    }

    return mCurrent->first;
}


KeyFileSettingsIterator KeyFileSectionsIterator::getSettingsIterator () const {
    if (!mCurrentIsElement) {
        throw KeyFileOutOfRangeException();
    }

    return KeyFileSettingsIterator(mCurrent->second->begin(),
                             mCurrent->second->end(),
                             *mCurrent->second);
}



KeyFile::KeyFile (const std::string &filename) {
    std::ifstream file(filename.c_str());

    if (file.bad() || file.eof() || file.fail()) {
        throw file_not_found(std::string("KeyFile: failed to load ")
                             + filename);
    }

    std::string str;
    std::shared_ptr<KeyFileSettings> current_settings(new KeyFileSettings());
    std::string current_section_name("");

    mSections.insert(
        KeyFileSections::value_type(current_section_name,
                              current_settings));

    while(!StringUtils::safeGetline(file, str).eof() || !file.fail()) {
        str = StringUtils::trim(str, " \t");

        if (str.length() > 0 && str.at(0) != '#') {
            if (str.at(0) == '[' && str.at(str.length() - 1) == ']') {
                current_section_name = str.substr(1, str.length() - 2);
                current_settings.reset(new KeyFileSettings());

                mSections.insert(
                            KeyFileSections::value_type(current_section_name,
                                                  current_settings));
            }
            else {
                std::string::size_type separator_pos =
                        str.find_first_of(" \n\0", 0);
                std::string val_name = str.substr(0, separator_pos);
                std::string val =
                        (separator_pos + 1 == std::string::npos
                         || separator_pos == std::string::npos)
                        ? ""
                        : str.substr(separator_pos + 1);

                current_settings->insert(
                            KeyFileSettings::value_type(val_name, val));
            }
        }
    }
}




KeyFile::~KeyFile () {
}


KeyFileSectionsIterator KeyFile::getSectionsIterator () const{
    return KeyFileSectionsIterator(mSections.cbegin(), mSections.cend());
}

size_t KeyFile::sectionCount (const std::string &pSectionName) const {
    return mSections.count(pSectionName);
}

KeyFileSettingsIterator
KeyFile::getLastSectionSettings (const std::string &pSectionName) const {
    std::shared_ptr<KeyFileSettings> settings(
                (--mSections.upper_bound(pSectionName))->second);
    return KeyFileSettingsIterator(settings->begin(),
                             settings->end(),
                             *settings);
}

void KeyFile::appendKey (const std::string &section,
                          const std::string &key,
                          const std::string &value) {
    if (mSections.count(section) > 1) {
        throw std::runtime_error(
            std::string("can't append key to ambiguous section ")
            + section);
    }

    std::shared_ptr<KeyFileSettings> settings;

    if (mSections.find(section) == mSections.end()) {
        settings.reset(new KeyFileSettings());

        mSections.insert(
            KeyFileSections::value_type(section, settings));
    }
    else {
        KeyFileSections::const_iterator it = mSections.find(section);
        settings = (*it).second;
    }

    (*settings)[key] = value;
}


void KeyFile::appendKey (const std::string &key, const std::string &value) {
    assert (!key.empty());
    assert (!value.empty());

    std::string::size_type separator_pos =
        key.find_first_of(".", 0);

    const std::string section_name =
        (separator_pos + 1 == std::string::npos
         || separator_pos == std::string::npos)
        ? ""
        : key.substr(0, separator_pos);

    const std::string key_name =
        (separator_pos < std::string::npos)
        ? key.substr(separator_pos + 1)
        : key;

    appendKey(section_name, key_name, value);
}


void KeyFile::writeToFile (const std::string &filename) {
    std::ofstream file(filename.c_str());
    KeyFileSectionsIterator sections = getSectionsIterator();

    for (;sections.isElement(); sections.peekNext()) {
        if (!sections.getName().empty()) {
            file << "[" << sections.getName() << "]" << std::endl;
        }

        KeyFileSettingsIterator settings = sections.getSettingsIterator();

        for (;settings.isElement(); settings.peekNext()) {
            file << settings.getName() << " "
                 << settings.getValue()
                 << std::endl;
        }
    }
}

} // namespace VcppBits
