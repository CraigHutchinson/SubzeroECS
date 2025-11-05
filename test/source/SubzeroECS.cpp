//#include <doctest/doctest.h>
#include <SubzeroECS/SubzeroECS.hpp>
#include <SubzeroECS/version.h>

#include <string>

TEST_CASE("SubzeroECS") {
  using namespace SubzeroECS;

  SubzeroECS SubzeroECS("Tests");

  CHECK(SubzeroECS.greet(LanguageCode::EN) == "Hello, Tests!");
  CHECK(SubzeroECS.greet(LanguageCode::DE) == "Hallo Tests!");
  CHECK(SubzeroECS.greet(LanguageCode::ES) == "¡Hola Tests!");
  CHECK(SubzeroECS.greet(LanguageCode::FR) == "Bonjour Tests!");
}

TEST_CASE("SubzeroECS version") {
  static_assert(std::string_view(SubzeroECS_VERSION) == std::string_view("1.0"));
  CHECK(std::string(SubzeroECS_VERSION) == std::string("1.0"));
}
