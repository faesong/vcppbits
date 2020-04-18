// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


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


#include <stdexcept>
#include <string>

#include "contrib/catch2/catch.hpp"

#include "Setting.hpp"

using namespace VcppBits;

namespace {

template <typename T>
void check_simple_init (T val,
                        T second_val,
                        std::string as_string,
                        Setting::VALUETYPE value_type,
                        Setting::TYPE setting_type) {
    Setting foo("foo", (const T) val);
    CHECK(foo.getValue<T>() == val);

    //    CHECK(foo.getDefaultValue() == val);
    CHECK(foo.isDefault());
    CHECK(foo.getType() == setting_type);
    CHECK(foo.getValueType() == value_type);

    CHECK(foo.getAsString() == as_string);
    CHECK(foo.getName() == "foo");

    foo.setValue(second_val);
    CHECK(foo.getValue<T>() == second_val);
}
} // namespace (anonymous)


TEST_CASE( "Setting initialized", "[Setting]" ) {
    REQUIRE(1 == 1);
    //CHECK_THROWS_AS(Setting{}, std::runtime_error);


    check_simple_init<bool>(true,
                            false,
                            "1",
                            Setting::BOOLEAN,
                            Setting::S_BOOL);
    check_simple_init<int>( 11,
                            -100,
                           "11",
                           Setting::INTEGER,
                           Setting::S_INTEGER);
    check_simple_init<float>(0.5f,
                             3.1415926f,
                             "0.5",
                             Setting::FLOATINGPOINT,
                             Setting::S_FLOATINGPOINT);
    // TODO: somehow, make it avoid a trap where func(string) overloaded
    // call results in calling func(bool)
    check_simple_init<std::string>("init",
                                   "secondary",
                                   "init",
                                   Setting::STRING,
                                   Setting::S_STRING);
}


TEST_CASE( "Bounded setting initialized", "[Setting]" ) {
    REQUIRE(1 == 1);
    //CHECK_THROWS_AS(Setting{}, std::runtime_error);

}
