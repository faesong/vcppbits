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


#ifndef VcppBits_MATH_UTILS_HPP_INCLUDED__
#define VcppBits_MATH_UTILS_HPP_INCLUDED__

namespace VcppBits {
namespace MathUtils {

template<typename I, typename L, typename H>
auto clamp (const I& pVal, const L &lo, const H &hi) ->decltype(pVal+lo+hi) {
    if (pVal < lo) {
        return lo;
    } else if (pVal > hi) {
        return hi;
    }
    return pVal;
}

template<typename T>
bool compareFloats (const T A, const T B) {
    const auto diff = A - B;
    return (diff < std::numeric_limits<T>::epsilon())
        && (-diff < std::numeric_limits<T>::epsilon());
}

template<class T>
bool isNan(T t)
{
    return t != t;
}

template<class T>
T sign (const T& pVal) {
    return pVal > 0 ? T{1} : T{-1};
}

} // namespace MathUtils
} // namespace VcppBits

#endif // VcppBits_MATH_UTILS_HPP_INCLUDED__
