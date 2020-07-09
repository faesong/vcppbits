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

#include <locale>
#include <codecvt>


#include "Ids.hpp"
#include "Translation.hpp"

namespace VcppBits {
namespace Translation {


using StringUtils::toUtf8;

StringIds::StringIds ()
  : mStrings ({
#include "TranslationIdsStrings.hpp"
      }) {
}




std::string StringIds::toString (const Ids id) {
    if (id < mStrings.size()) {
        return mStrings[id];
    }
    return "___ERROR___";
}

Ids StringIds::fromString (const std::string &pString) {
    auto it = std::find(mStrings.begin(), mStrings.end(), pString);
    if (it != mStrings.end()) {
        return (Ids) std::distance(mStrings.begin(), it);
    }
    return Ids::_ILLEGAL_ELEMENT_;
}

std::vector<std::string> validate_translation_string (const std::string &pEng,
                                                      const std::string &pTr) {
    StringUtils::Tokenizer tok (pEng, " :");
    std::vector<std::string> subs;
    std::vector<std::string> not_found_subs;
    for (const auto &el : tok.getResultList()) {
        if (el[0] == '%') {
            subs.push_back(el);
        }
    }

    for (const auto &el : subs) {
        if (pTr.find(el) != std::string::npos) {
            not_found_subs.push_back(el);
        }
    }

    return not_found_subs;
}


Translation::Translation () {
    mData["English"] = std::map<Ids, std::string>{
#include "TranslationEnglishDefault.hpp"
    };
    mLanguages = { "English" };


    const std::string dir = "Data/Langs";

    if (!fs::is_directory(dir)
        && !fs::create_directories(dir)) {
        throw std::runtime_error("couldn't create \"" + dir + "\" directory");
    }

    if (!fs::exists(dir + "/English.example.txt")) {
        dumpTranslationData();
    }

    for (auto& p: fs::directory_iterator(dir)) {
        const std::string &file_name = toUtf8(p.path().filename());
        const std::string ex_suffix = ".example.txt";
        if (!StringUtils::endsWith(file_name, ex_suffix)) {
            const std::string &pth = toUtf8(p.path().relative_path().native());
            loadTranslationFile(pth, file_name);
        }
    }
}


const std::string & Translation::get (const Ids pId,
                                      const std::string &pLang) const {
    if (mData.count(pLang) && mData.at(pLang).count(pId)) {
        return mData.at(pLang).at(pId);
    }
    // TODO: hardcoded English
    return mData.at("English").at(pId);
}

const std::string & Translation::getExact (const Ids pId,
                                           const std::string &pLang) const {
    static const std::string empty = "";
    if (mData.count(pLang) && mData.at(pLang).count(pId)) {
        return mData.at(pLang).at(pId);
    }
    return empty;
}

const std::vector<std::string> &Translation::getLanguages () const {
    return mLanguages;
}

void Translation::dumpTranslationData () {
    std::ofstream file ("Data/Langs/English.example.txt");
    for (const auto &el : mData["English"]) {
        file << mIds.toString(el.first)
             << " "
             << el.second << '\n';
    }
}

void Translation::loadTranslationFile (const std::string &pFile,
                                       const std::string &pLang) {
    std::string language_name = pLang;

    std::ifstream file(pFile);

    std::map<Ids, std::string> dt;

    std::string str;
    while(!StringUtils::safeGetline(file, str).eof() || !file.fail()) {
        str = StringUtils::trim(str, " \t");

        if (str.length() > 0 && str.at(0) != '#') {
            std::string::size_type separator_pos =
                str.find_first_of(" \n\0", 0);

            std::string name = str.substr(0, separator_pos);
            std::string tr =
                (separator_pos + 1 == std::string::npos
                 || separator_pos == std::string::npos)
                ? ""
                : str.substr(separator_pos + 1);


            try {
                const Ids id = mIds.fromString(name.data());
                if (id != Ids::_ILLEGAL_ELEMENT_) {
                    dt[id] = tr;
                }
            } catch (std::runtime_error &err) {
                (void) err;
            }
        }
    }

    if (dt.find(Ids::TRANSLATION_LANGUAGE_NAME) != dt.end()) {
        language_name = dt.at(Ids::TRANSLATION_LANGUAGE_NAME);
    }

    if (std::find(mLanguages.begin(), mLanguages.end(), language_name)
        == mLanguages.end()) {
        mLanguages.push_back(language_name);
    }

    mData[language_name] = dt;
}

} // namespace Translation
} // namespace VcppBits
