#define CATCH_CONFIG_MAIN
#include "../vendor/catch.hpp"

#include "./score.h"

TEST_CASE("ACheck: punctuation and capital letters scoring", "[ACheck]") {
    REQUIRE(ACheck(0, "Hello world.") == 20);
    std::string c = std::string("Hello world!");
    REQUIRE(ACheck(0, c) == 20);
    REQUIRE(c == "Hello world!"); // ensure string was not modified
    REQUIRE(ACheck(0, "Hello world?") == 20);
    REQUIRE(ACheck(0, "Hello world. This is a test.") == 30);
    REQUIRE(ACheck(0, "Hello world. this is a test.") == 10);
    REQUIRE(ACheck(0, "") == 0);
}

TEST_CASE("BCheck: trigram scoring", "[BCheck]") {
    std::string body = "the quick brown fox";
    REQUIRE(BCheck("english", 0, body) == 9); // Assuming "the", "qui", "bro" are trigrams
    body = "le rapide renard brun";
    REQUIRE(BCheck("french", 0, body) == 6); // Assuming "rap", "ren" are trigrams
}

TEST_CASE("CCheck: first character capitalization scoring", "[CCheck]") {
    std::string body = "Hello world";
    REQUIRE(CCheck(0, body) == 20);
    body = "hello world";
    REQUIRE(CCheck(0, body) == -10);
}

TEST_CASE("DCheck: repeated character penalty", "[DCheck]") {
    std::string body = "Hellooo world";
    REQUIRE(DCheck(0, body) == -50);
    body = "Hello world";
    REQUIRE(DCheck(0, body) == 0);
}

TEST_CASE("ECheck: space ratio scoring", "[ECheck]") {
    std::string body = "Hello world";
    REQUIRE(ECheck(0, body) == 20);
    body = "Helloworld";
    REQUIRE(ECheck(0, body) == 20);
    body = "     ";
    REQUIRE(ECheck(0, body) == -20);
}

TEST_CASE("FCheck: long text without punctuation penalty", "[FCheck]") {
    std::string body = std::string(76, 'a');
    REQUIRE(FCheck(0, body) == -150);
    body = std::string(75, 'a') + ".";
    REQUIRE(FCheck(0, body) == 0);
}

TEST_CASE("GCheck: space in groups of 32 characters", "[GCheck]") {
    std::string body = std::string(31, 'a') + "  " + std::string(31, 'b');
    REQUIRE(GCheck(0, body) == 0);
    body = std::string(64, 'a');
    REQUIRE(GCheck(0, body) == -40);
}
