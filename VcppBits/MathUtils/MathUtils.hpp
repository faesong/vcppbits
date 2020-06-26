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

#include <cassert>
#include <vector>

//TODO2: cleanup
#include <iostream>
using namespace std;

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

template<class T>
T sqr(const T& pVal) {
    return pVal * pVal;
}

/// round integer up to closest multiple
template<typename T>
T roundUp (T pNumToRound, T pMultiple) {
    T isPositive = (T)(pNumToRound >= 0);
    return ((pNumToRound + isPositive * (pMultiple - 1)) / pMultiple) * pMultiple;
}

template<typename T>
T getClosestMultiple (T pNum, T pMultipleOf) {
    // https://stackoverflow.com/a/44110705
    const T mod = pNum % pMultipleOf;
    T round = pNum - mod;
    if (mod > 0) {
        round += pMultipleOf;
    }
    return round;
}

template <typename T> T interp_bezier (T p_0,
                                       T p_1,
                                       T p_2,
                                       float k) {
    using VcppBits::MathUtils::sqr;

    return sqr(1.f - k) * p_0 + 2.f * k * (1.f - k) * p_1 + sqr(k) * p_2;
}

namespace detail {
// 1. client code supplies N segments
// 2. we turn them to N-1 segments, like so
//    (1) (0th) origin <=> middle of second (1th in computer world) segment
//    (2) middle of second segment <=> middle of third
//    ...
//    last segment is middle of second-to-last <=> end of the last segment
//
// that way we can have simple quadratic bezier calculation where first and last
// points stay on the same place, and all points in between are smoothed
inline float nurbs_clientside_k_to_nurbs_k (const float pK,
                                            const size_t pNumSegments) {
    assert(pNumSegments > 2);
    if (pK < 1.5f) {
        return pK * 2.f / 3.f;
    } else if (pK > float(pNumSegments - 2) + 0.5) {
        return (pK - (pNumSegments - 2) - 0.5)/ 3.f * 2.f + (pNumSegments - 2);
    }
    return pK - .5f;
}
}

template<typename DataT, typename StorageT, typename AdpT>
class NurbsCurveWalker {
public:
    explicit NurbsCurveWalker (
        const std::vector<StorageT> &pSegments)
        : _segments(pSegments) {
    }


    // TODO20: cleanup debug prints
    DataT getPos (const float pPos) const {
        const float pInnerPos = detail::nurbs_clientside_k_to_nurbs_k(pPos, _segments.size() + 1);
        // cout << "pPos " << pPos << endl;
        // cout << "pInnerPos " << pInnerPos << endl;
        const float diff = pInnerPos - _pos;
        const int to_walk = int(diff);
        const int incr = to_walk ? to_walk / abs(to_walk) : 0;

        //cout << "will walk from " << 0 << " to " << to_walk
         //    << " with incr " << incr << endl;

        DataT ret{};

        size_t pos = size_t(_pos);
        size_t dest = size_t(std::min(pInnerPos, float(_segments.size() - 2)));
        float leftover = std::min(pInnerPos - dest, 1.f);
        // cout << "dest " << dest << endl;
        for (; pos != dest; pos = size_t(int(pos) + incr)) {
            // cout << "walking" << incr << " " << pos << endl;
            ret = ret + _adp.get(_segments[pos]);
        }

        const DataT vec = _adp.get(_segments[dest]);
        const DataT vec2 = _adp.get(_segments[dest+1]);
        //cout << "dbg" << vec << vec2 << endl;
        //cout << "ret_init" << ret << endl;

        const auto p_0 = ((dest == 0) ? DataT{} : vec / 2.f);
        const auto p_1 = vec;
        const auto p_2 = vec + ((dest == _segments.size() - 2) ? vec2  : (vec2 / 2.f));

        // cout << p_0 << " " << p_1 << " " << p_2 << " " << pInnerPos - dest << endl;
        auto rrr = interp_bezier(p_0, p_1, p_2, leftover);
        // cout << "interp: " << rrr << endl;
        ret = ret + rrr;
        return ret;
    }

    void setReferencePos (const float pReferencePos) {
        _pos = pReferencePos;
        // TODO
    }

    float getReferencePos () const {
        return _pos;
    }

private:
    AdpT _adp;
    const std::vector<StorageT> &_segments;
    float _pos = 0.f;
};


} // namespace MathUtils
} // namespace VcppBits

#endif // VcppBits_MATH_UTILS_HPP_INCLUDED__
