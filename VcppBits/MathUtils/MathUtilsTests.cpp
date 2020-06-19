// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


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


#include "contrib/catch2/catch.hpp"
#include "MathUtils.hpp"

using namespace VcppBits::MathUtils;

struct SimpleVector3 {
    float x, y, z;
    bool operator== (const SimpleVector3& pOther) const {
        return x==pOther.x && y == pOther.y && z == pOther.z;
    }

    SimpleVector3 operator+ (const SimpleVector3& pOther) const {
        return { x + pOther.x,
                 y + pOther.y,
                 z + pOther.z };
    }

    SimpleVector3 operator/ (const float pDenominator) const {
        return { x / pDenominator,
                 y / pDenominator,
                 z / pDenominator };
    }

    SimpleVector3 operator* (const float pMultiplier) const {
        return { x * pMultiplier,
                 y * pMultiplier,
                 z * pMultiplier };
    }
};

SimpleVector3 operator * (float pMultiplier, const SimpleVector3 &pVec) {
    return { pVec.x * pMultiplier,
             pVec.y * pMultiplier,
             pVec.z * pMultiplier };
}



struct SimpleVector3Adp {
    const SimpleVector3 &get (const SimpleVector3& pVec) const {
        return pVec;
    }
};

struct ApproxSV3 {
    explicit ApproxSV3 (const SimpleVector3 &pVec)
        : _vec (pVec) {
    }

    SimpleVector3 _vec;
};

ostream& operator<< (ostream& os, const SimpleVector3& pVec) {
    os << "{ " << std::fixed << pVec.x << ' ' << pVec.y << ' ' << pVec.z << " }";
    return os;
}


namespace Catch {
    template<> struct StringMaker<ApproxSV3> {
        static std::string convert(ApproxSV3 const& a) {
            return "{ " + std::to_string(a._vec.x) + " " +
                std::to_string(a._vec.y) + " " +
                std::to_string(a._vec.z) + " }";
        }
    };
}


template <typename FP_T>
bool my_approx_eq (const FP_T pPrecision, const FP_T pVal1, const FP_T pVal2) {
    if (compareFloats(pVal1, pVal2)) {
        return true;
    }
    float fraction = compareFloats(pVal2, 0.f) ? pVal2/pVal1 : pVal1/pVal2;
    return (abs(fraction - FP_T{1.0f}) < pPrecision);
}

bool operator== (const ApproxSV3 &pAppr, const SimpleVector3 &pOther) {
    constexpr float prec = std::numeric_limits<float>::epsilon() * 2;
    return my_approx_eq(prec, pOther.x, pAppr._vec.x)
        && my_approx_eq(prec, pOther.y, pAppr._vec.y)
        && my_approx_eq(prec, pOther.z, pAppr._vec.z);
}

bool operator== (const SimpleVector3 &pOther, ApproxSV3 const &pAppr) {
    return pAppr == pOther;
}


TEST_CASE("nurbs_clientside_k_to_nurbs_k", "[NurbsCurveWalker]") {
    auto const f = detail::nurbs_clientside_k_to_nurbs_k;
    REQUIRE(f(1.f, 5) == Approx(2.f/3.f));

    REQUIRE(f(2.f, 5) == Approx(1.5f));
    REQUIRE(f(3.f, 5) == Approx(2.5f));

    REQUIRE(f(3.5f, 5) == Approx(3.f));
    REQUIRE(f(4.f, 5) == Approx(3.f + 1.f / 3.f));
    REQUIRE(f(5.f, 5) == Approx(4.f));
}


TEST_CASE("nbmn,bmnj", "[NurbsCurveWalker]") {
    auto const f = detail::nurbs_clientside_k_to_nurbs_k;
    REQUIRE(f(0.f, 8) == Approx(0.f));
    REQUIRE(f(1.5f, 8) == Approx(1.f));
    REQUIRE(f(2.5f, 8) == Approx(2.f));
    REQUIRE(f(3.5f, 8) == Approx(3.f));
    REQUIRE(f(4.5f, 8) == Approx(4.f));
    REQUIRE(f(5.5f, 8) == Approx(5.f));
    REQUIRE(f(6.5f, 8) == Approx(6.f));
}

TEST_CASE("NurbsCurveWalker initialized", "[NurbsCurveWalker]" ) {
    std::vector<SimpleVector3> dt {
                                   { 0, 1, 1 },
                                   { 0, 1, 1 },
                                   { 0, 1, 1 },
                                   { 0, 1, 1 },
    };
    //cout << ApproxV3(SimpleVector3{ 0,1,2 }) << endl;
    NurbsCurveWalker<SimpleVector3, SimpleVector3, SimpleVector3Adp> walker(dt);
    REQUIRE(walker.getPos(1.f) == ApproxSV3(SimpleVector3{0,1.1111111,1.111111}));
    REQUIRE(walker.getPos(2.f) == ApproxSV3(SimpleVector3{0,2,2}));
    REQUIRE(walker.getPos(1.5f) == ApproxSV3(SimpleVector3{0,1.5f,1.5f }));
    REQUIRE(walker.getReferencePos() == 0.f);
    REQUIRE(walker.getPos(0.f) == ApproxSV3(SimpleVector3{0,0,0}));

    // overflow
    REQUIRE(walker.getPos(6.f) == ApproxSV3(SimpleVector3{0,4,4}));
    REQUIRE(walker.getPos(4.f) == ApproxSV3(SimpleVector3{0,4,4}));
    // REQUIRE(walker.getPos(2.f) == ApproxSV3(SimpleVector3{0.f,3.f,11.f}));
    // walker.setReferencePos(2.1f);
    // REQUIRE(walker.getPos(2.f) == ApproxSV3(SimpleVector3{0,0,0}));
    // REQUIRE(walker.getPos(0.f) == ApproxSV3(SimpleVector3{0,0,0}));
}

TEST_CASE("NurbsCurveWalker plot", "[NurbsCurveWalker]" ) {
    
    std::vector<SimpleVector3> dt {};
    for (int i = 0; i < 50; ++i) {
        dt.push_back(SimpleVector3{ 1, 1, 1 });
    }

    NurbsCurveWalker<SimpleVector3, SimpleVector3, SimpleVector3Adp> walker(dt);

    for (int i = 2; i < 49; ++i) {
        CHECK(walker.getPos(float(i)) == ApproxSV3(SimpleVector3{float(i),
                                                                 float(i),
                                                                 float(i)}));
    }
    
}
