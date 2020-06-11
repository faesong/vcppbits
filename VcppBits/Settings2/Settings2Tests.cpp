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
#include "Settings2.hpp"

using namespace V2;
using namespace V2::SettingsDefault;

TEST_CASE("Setting2 initialized", "[Setting2]" ) {
    Setting s1(BoolValue(false));
    REQUIRE(s1.get<BoolValue>() == false);
    Setting s2(FloatValue(0.f));
    REQUIRE(s2.get<FloatValue>() == Approx(0.f));
    REQUIRE_THROWS_AS(s2.get<BoolValue>(), std::bad_variant_access);
}

TEST_CASE("Setting2TypeEnum checked", "[Setting2]" ) {
    REQUIRE(Setting(BoolValue(false)).getType() == Setting::Type::BOOL);
    REQUIRE(Setting(IntValue(0)).getType() == Setting::Type::INT);
    REQUIRE(Setting(FloatValue(0.f)).getType() == Setting::Type::FLOAT);
    REQUIRE(Setting(EnumIntValue(0,
                                 EnumConstraint<int>({ 0 })))
            .getType()
            ==
            Setting::Type::ENUM_INT);
    REQUIRE(Setting(EnumFloatValue(0.f,
                                   EnumConstraint<float>({ 0.f })))
                .getType()
            ==
            Setting::Type::ENUM_FLOAT);
    REQUIRE(Setting(StringValue("")).getType() == Setting::Type::STRING);
    REQUIRE(Setting(EnumStringValue("",
        EnumConstraint<std::string>({ "" })))
            .getType()
            ==
            Setting::Type::ENUM_STRING);
}


TEST_CASE("Arithmetically constrained setting initialized", "[Setting2]" ) {
    Setting s1(BoolValue(false));
    REQUIRE(s1.get<BoolValue>() == false);
    Setting s2(FloatValue(1.2f, ArithmeticConstraint(0.3f, 1.4f)));
    REQUIRE(s2.get<FloatValue>() == Approx(1.2f));
    REQUIRE_THROWS_AS(Setting(FloatValue(1.2f, ArithmeticConstraint(0.f, 1.f))),
                      std::runtime_error);
}

TEST_CASE("Enum constrained setting initialized", "[Setting2]" ) {
    Setting s(EnumFloatValue(.3f, EnumConstraint<float>({ 0.3f, 1.4f, 1.9f })));
    REQUIRE(s.get<EnumFloatValue>() == Approx(0.3f));
    REQUIRE_THROWS_AS(Setting(EnumFloatValue(1.2f, EnumConstraint<float>({0.f, 1.f}))),
                      std::runtime_error);
    Setting s2(EnumIntValue(1, EnumConstraint<int>({ 0, 1 })));

    REQUIRE(s2.get<EnumIntValue>() == 1);
}

TEST_CASE("Setting2 converted to string", "[Setting2]" ) {
    Setting s(EnumFloatValue(.3f, EnumConstraint<float>({ 0.3f, 1.4f, 1.9f })));
    Setting s2(EnumIntValue(1, EnumConstraint<int>({ 0, 1 })));

    REQUIRE(s.getAsString() == "0.300000");
    REQUIRE(s2.getAsString() == "1");
    REQUIRE(Setting(BoolValue(false)).getAsString() == "0");
    REQUIRE(Setting(IntValue(1)).getAsString() == "1");
    REQUIRE(Setting(FloatValue(1.123f)).getAsString()
            ==
            "1.123000");
    REQUIRE(Setting(EnumFloatValue(3.1415926f,
                                   EnumConstraint<float>({0.f, 3.14159261f})))
            .getAsString()
            == "3.141593");
}

TEST_CASE("Setting2 of string initialized", "[Setting2]" ) {
    Setting s(StringValue(std::string("hehe")));
    REQUIRE(s.getAsString() == "hehe");
    REQUIRE(std::string(s.get<StringValue>()) == "hehe");
}

TEST_CASE("Enum setting of string initialized", "[Setting2]") {
    Setting s(EnumStringValue("hehe", EnumConstraint<std::string>({ "hehe",
                                                                  "haha",
                                                                  "ololo" })));
    REQUIRE(s.getAsString() == std::string("hehe"));
    // TODO: add to API
    //REQUIRE(s.get<EnumStringValue>().getConstraint().isValid("haha"));

    REQUIRE_THROWS_AS(Setting(EnumStringValue("hoho", EnumConstraint<std::string>({ "hehe",
                                                                  "haha",
                                                                  "ololo" }))),
        std::runtime_error);
}

#include <iostream>
#include <fstream>


TEST_CASE("Read some settings", "[Settings2]") {
    const auto filename = "test_Settings_0.txt";
    using namespace std;
    {
        ofstream myfile;
        myfile.open (filename);
        myfile << "# comment.\n"
               << "toplevel_str one\n"
               << "toplevel_int 1241\n"
               << "toplevel_float 3.1415926\n"
               << "[section1]\n"
               << "foo a bit longer string\n"
               << "bar_int 12312\n";
    }

    Settings settings(filename);
    settings.appendSetting("toplevel_str",
                           StringValue(std::string("default_val")));
    settings.appendSetting("toplevel_int",
                           IntValue(int(0)));
    settings.appendSetting("toplevel_float",
                           FloatValue(float(0.1)));
    settings
        .appendSetting("section1.foo",
                       StringValue("default_str"));
    settings
        .appendSetting("section1.bar_int",
                       EnumIntValue(0,
                                    EnumConstraint<int>({ 0, 1, 12312 })));

    REQUIRE(settings.get<StringValue>("toplevel_str") == "default_val");
    REQUIRE(settings.get<IntValue>("toplevel_int") == 0);
    REQUIRE(settings.get<FloatValue>("toplevel_float") == Approx(0.1f));
    REQUIRE(settings.get<StringValue>("section1.foo") == "default_str");
    REQUIRE(settings.get<EnumIntValue>("section1.bar_int") == 0);

    settings.load();

    REQUIRE(settings.get<StringValue>("toplevel_str") == "one");
    REQUIRE(settings.get<IntValue>("toplevel_int") == 1241);
    REQUIRE(settings.get<FloatValue>("toplevel_float") == Approx(3.1415926f));
    REQUIRE(settings.get<StringValue>("section1.foo") == "a bit longer string");
    REQUIRE(settings.get<EnumIntValue>("section1.bar_int") == 12312);
}

TEST_CASE("Setting2 ptr update mechanism", "[Setting2]" ) {
    std::string keep_me_updated;
    Settings cfg;
    auto &s = SettingsUtils::create<StringValue>(cfg, "noname", "hehe", &keep_me_updated);
    REQUIRE(keep_me_updated == s.get<StringValue>());
    s.set<StringValue>("xoxo");
    REQUIRE(keep_me_updated == s.get<StringValue>());
}
