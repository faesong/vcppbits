#include <string>
#include <iostream>

#include "VcppBits/KeyFile/KeyFile.hpp"


int main () {
    using namespace VcppBits;

    std::string a_setting = "default_string";

    try {
        KeyFile f ("config.ini");
        try {
            a_setting = f.getLastSectionSettings("").findSetting("a_setting");
        } catch (KeyFileSettingNotFoundException &) {
        }
    } catch (KeyFile::file_not_found &) {
    }

    std::cout << "setting is: " << a_setting << std::endl;

    return 0;
}
