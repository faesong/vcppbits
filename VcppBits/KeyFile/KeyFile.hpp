// The MIT License (MIT)

// Copyright 2020 Vitalii Minnakhmetov <restlessmonkey@ya.ru>

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


#ifndef VcppBits_KEY_FILE_HPP_INCLUDED__
#define VcppBits_KEY_FILE_HPP_INCLUDED__

#include <map>
#include <stdexcept>
#include <memory>

namespace VcppBits {

typedef std::map<std::string, std::string> KeyFileSettings;
typedef std::multimap<std::string,
                      std::shared_ptr<KeyFileSettings>> KeyFileSections;

class KeyFileOutOfRangeException {};
class KeyFileSettingNotFoundException {};

class KeyFileSettingsIterator {
public:
    KeyFileSettingsIterator (KeyFileSettings::iterator b,
                             KeyFileSettings::iterator e,
                             KeyFileSettings &settings);

    void peekNext ();
    bool isElement () const;

    KeyFileSettings::value_type getSetting () const;
    std::string getName () const;
    std::string getValue () const;

    std::string findSetting (const std::string &name) const;

private:
    const KeyFileSettings::iterator mBegin;
    const KeyFileSettings::iterator mEnd;
    KeyFileSettings::iterator mCurrent;

    KeyFileSettings &mSettings;

    bool mCurrentIsElement;
};

class KeyFileSectionsIterator {
public:
    KeyFileSectionsIterator (const KeyFileSections::const_iterator b,
                             const KeyFileSections::const_iterator e);

    void peekNext ();
    bool isElement () const;

    std::string getName() const;

    KeyFileSettingsIterator getSettingsIterator () const;
private:
    const KeyFileSections::const_iterator mBegin;
    const KeyFileSections::const_iterator mEnd;
    KeyFileSections::const_iterator mCurrent;
    bool mCurrentIsElement;
};

class KeyFile {
public:
    class file_not_found : public std::runtime_error {
    public:
        file_not_found (const std::string &message)
            : std::runtime_error (message.c_str()) {
        }
    };

    KeyFile (const std::string &filename);
    KeyFile () {
        std::shared_ptr<KeyFileSettings> current_settings(new KeyFileSettings());
        std::string current_section_name("");

        mSections.insert(
            KeyFileSections::value_type(current_section_name,
                                        current_settings));
    };
    ~KeyFile ();

    KeyFileSectionsIterator getSectionsIterator () const;

    size_t sectionCount (const std::string &pSectionName) const;
    KeyFileSettingsIterator
        getLastSectionSettings (const std::string &pSectionName) const;

    void appendKey (const std::string &section,
                    const std::string &key,
                    const std::string &value);
    // key format here is section.key_name or just key_name
    void appendKey (const std::string &key,
                    const std::string &value);

    void writeToFile (const std::string &filename);
private:
    KeyFileSections mSections;
};

} // namespace VcppBits

#endif // VcppBits_KEY_FILE_HPP_INCLUDED__
