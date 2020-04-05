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
