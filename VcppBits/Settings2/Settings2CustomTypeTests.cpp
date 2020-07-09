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


#include <iomanip>
#include <iostream>

#include "VcppBits/contrib/catch2/catch.hpp"
#include "Settings2.hpp"

using namespace V2;

struct Vector3 {
    float x, y, z;

    bool operator >= (const Vector3 &pOther) const {
        return x >= pOther.x && y >= pOther.y && z >= pOther.z;
    }

    bool operator <= (const Vector3 &pOther) const {
        return x <= pOther.x && y <= pOther.y && z <= pOther.z;
    }

    // for our own convenience
    static constexpr float _lowest = std::numeric_limits<float>::lowest();
    static constexpr float _max = std::numeric_limits<float>::max();
    static Vector3 lowest () { return Vector3{ _lowest, _lowest, _lowest }; };
    static Vector3 max () { return Vector3{ _max, _max, _max }; };
};


/// Example on how to use library with your own type

// TODO3: consider being able to specify default constraint value when
// specializing SettingValue

inline std::string vector3_to_string (const Vector3& pVector3) {
    std::stringstream ss;
    ss << std::fixed;
    ss << "Vector3(" << pVector3.x << " "
        << pVector3.y << " "
        << pVector3.z << ")";
    return ss.str();
}

inline Vector3 vector3_from_string (const std::string &pStr) {
    using VcppBits::StringUtils::fromString;
    if (pStr.find("Vector3(") == 0
        && pStr[pStr.size() - 1] == ')') {
        Vector3 ret;
        std::stringstream ss(pStr.substr(8, pStr.size() - 1));
        ss >> ret.x >> ret.y >> ret.z;

        return ret;
    }

    throw std::runtime_error("couldn't parse Vector3 from: " + pStr);
}

using Vector3Value = SettingValue<Vector3, GenericArithmeticConstraint, vector3_to_string, vector3_from_string>;

enum class SettingTypeEnum : std::size_t { BOOL,
                                           INT,
                                           FLOAT,
                                           ENUM_INT,
                                           ENUM_FLOAT,
                                           STRING,
                                           ENUM_STRING,
                                           VECTOR3 };

using Setting = SettingImpl<SettingTypeEnum,
                            BoolValue,
                            IntValue,
                            FloatValue,
                            EnumIntValue,
                            EnumFloatValue,
                            StringValue,
                            EnumStringValue,
                            Vector3Value>;

using Settings = SettingsImpl<Setting>;



// tests utility
static const auto vec3_constr =
    GenericArithmeticConstraint<Vector3>(Vector3::lowest(), Vector3::max());

struct ApproxV3 {
    explicit ApproxV3 (const Vector3 &pVec)
        : vec (pVec) {
    }

    Vector3 vec;
};

bool operator== (const ApproxV3 &pAppr, const Vector3 &pOther) {
    return pOther.x == Approx(pAppr.vec.x)
        && pOther.y == Approx(pAppr.vec.y)
        && pOther.z == Approx(pAppr.vec.z);
}

bool operator== (const Vector3 &pOther, const ApproxV3 &pAppr) {
    return pAppr == pOther;
}

TEST_CASE("Test our custom Vector3 utilities", "[Vector3]") {
    const auto a_vec = Vector3{ 0.f, 3.141593f, 1000000.f };
    const auto a_str = "Vector3(0.000000 3.141593 1000000.000000)";

    REQUIRE(vector3_to_string(a_vec) == a_str);
    REQUIRE(vector3_from_string(a_str) == ApproxV3(a_vec));

    REQUIRE(vector3_from_string(vector3_to_string(a_vec)) == ApproxV3(a_vec));
    REQUIRE(vector3_to_string(vector3_from_string(a_str)) == a_str);
}

TEST_CASE("Custom Setting initialized", "[Vector3Value]") {

    Setting s(Vector3Value({0.1f, 0.f, 0.f}, vec3_constr));
    REQUIRE(s.get<Vector3Value>() == ApproxV3(Vector3{0.1f, 0.f, 0.f}));
    REQUIRE(s.getAsString() == "Vector3(0.100000 0.000000 0.000000)");
    s.setByString("Vector3(0.123456 0.789101 0.111213)");
    REQUIRE(s.get<Vector3Value>()
            ==
            ApproxV3(Vector3{ 0.123456, 0.789101, 0.111213 }));
}
