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


#ifndef VcppBits_STRING_UTILS_HPP_INCLUDED__
#define VcppBits_STRING_UTILS_HPP_INCLUDED__


#include <string>
#include <algorithm>
#include <sstream>


namespace VcppBits {
namespace StringUtils {

inline std::istream& safeGetline (std::istream& is, std::string& t) {
    // Taken from http://stackoverflow.com/a/6089413/4294001
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}


inline std::string trim (const std::string &pString,
                         const std::string &pWhitespace = " \t") {
    const size_t beginStr = pString.find_first_not_of(pWhitespace);

    if (beginStr == std::string::npos) {
        // no content
        return "";
    }

    const size_t endStr = pString.find_last_not_of(pWhitespace);
    const size_t range = endStr - beginStr + 1;
    return pString.substr(beginStr, range);
}

inline std::string reduce (const std::string &pString,
                           const std::string &pFill = " ",
                           const std::string &pWhitespace = " \t") {
    // trim first
    std::string result(trim(pString, pWhitespace));
    // replace sub ranges
    size_t beginSpace = result.find_first_of(pWhitespace);

    while (beginSpace != std::string::npos) {
        const size_t endSpace =
            result.find_first_not_of(pWhitespace, beginSpace);
        const size_t range = endSpace - beginSpace;
        result.replace(beginSpace, range, pFill);
        const size_t newStart = beginSpace + pFill.length();
        beginSpace = result.find_first_of(pWhitespace, newStart);
    }

    return result;
}


inline std::string capitalized (const std::string &str) {
    std::string ret_str = str;

    if (ret_str.length() > 0) {
        ret_str[0] = toupper(ret_str[0]);
    }
    return ret_str;
}

inline void capitalize (std::string &str) {
    str[0] = toupper(str[0]);
}


template <typename T>
inline T fromString (const std::string &str) {
    std::stringstream ss_val(str);
    T ret_val;
    ss_val >> ret_val;
    return ret_val;
}

template <typename T>
inline std::string toString (const T &val) {
    std::stringstream ss_val;
    ss_val << val;
    return ss_val.str();
}

template <>
inline std::string toString (const float &pVal) {
    std::stringstream ss;
    ss << pVal;
    auto str = ss.str();
    size_t i = str.size();

    if (str.find('.') != std::string::npos) {
        for (; i > 0; --i) {
            if (str[i-1] != '0') {
                break;
            }
        }
    }

    return str.substr(0, i);
}


} // namespace StringUtils
} // namespace VcppBits


#endif // VcppBits_STRING_UTILS_HPP_INCLUDED__
