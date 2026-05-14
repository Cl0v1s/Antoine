#define CATCH_CONFIG_MAIN
#include "../vendor/catch.hpp"

#include <string>
#include "./translator.h"

TEST_CASE("Chars are decoded", "[translator]" ) {
    const char ctest1[] = { 0x1b, 0x1c, 0x1d, 0x0 };
    std::string test1(ctest1);
    REQUIRE(decode(test1, false, false) == std::string("abc"));
}

TEST_CASE("Chars are encoded", "[translator]" ) {
    const char ctest1[] = { 0x1b, 0x1c, 0x1d, 0x0 };
    std::string test1("abc");
    std::string encoded = encode(test1, false, false);
    const char* result = encoded.c_str();
    REQUIRE(
        strcmp(
            result,
            ctest1
        ) == 0
    );
}

