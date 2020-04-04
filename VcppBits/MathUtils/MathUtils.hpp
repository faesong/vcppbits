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

} // namespace MathUtils
} // namespace VcppBits

#endif // VcppBits_MATH_UTILS_HPP_INCLUDED__
