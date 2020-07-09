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


#pragma once

#include <map>
#include <vector>
#include <string>
#include <cctype>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <algorithm>

#include <VcppBits/StringUtils/StringUtils.hpp>

namespace VcppBits {

namespace Translation {

namespace fs = std::filesystem;
namespace StringUtils = VcppBits::StringUtils;

class StringIds {
public:
    StringIds ();
    std::string toString (const Ids id);

    Ids fromString (const std::string &pString);

private:
    std::vector<std::string> mStrings;
};

std::vector<std::string> validate_translation_string (const std::string &pEng,
                                                      const std::string &pTr);

class Translation {
public:
    Translation ();

    const std::string& get (const Ids pId, const std::string &pLang) const;

    const std::string& getExact (const Ids pId,
                                 const std::string &pLang) const;

    const std::vector<std::string> &getLanguages () const;

    void dumpTranslationData ();

    /* TODO:
    void validateTranslations (Log &log) {
        const auto &en = mData.at("ENG");
        for (const auto &el: mData) {
            const auto &lang = el.first;
            if (lang == "ENG") {
                continue;
            }
            const auto &tr = el.second;

            for (const auto &trel : tr) {
                const auto id = trel.first;
                const auto &value = trel.second;
                const auto not_found =
                    validate_translation_string(en.at(id), value);
                if (not_found.size()) {

                    std::string str;
                    for (const auto &e : not_found) {
                        str += e + ' ';
                    }


                    log(Log::ERROR)
                        << lang << ' '
                        << mIds.toString(id) << ' '
                        << "not found:" << ' '
                        << str;

                }
            }
        }
    }
    */

private:
    void loadTranslationFile (const std::string &pFile,
                              const std::string &pLang);

    // Lang : (Id : String);
    std::map<std::string, std::map<Ids, std::string>> mData;
    std::vector<std::string> mLanguages;

    StringIds mIds;
};

class TranslationBinder {
public:
    TranslationBinder (const Translation &pTr,
                       const std::string &pLang)
        : mTr (pTr),
          mLang (pLang) {
    }
    const std::string& get (const Ids pId) const {
        return mTr.get(pId, mLang);
    }
    const std::string& getExact(const Ids pId) const {
        return mTr.getExact(pId, mLang);
    }
    void resetLang (const std::string& pLang) {
        mLang = pLang;
    }

private:
    const Translation &mTr;
    std::string mLang;
};


} // namespace Translation

} // namespace VcppBits
