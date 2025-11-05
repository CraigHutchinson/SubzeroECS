//#include <doctest/doctest.h>
#include <SubzeroECS/SubzeroECS.hpp>
#include <SubzeroECS/version.h>

#include <string>

TEST_CASE("SubzeroECS") {
  using namespace SubzeroECS;

  SubzeroECS ecs("Tests");

  CHECK(ecs.greet(LanguageCode::EN) == "Hello, Tests!");
  CHECK(ecs.greet(LanguageCode::DE) == "Hallo Tests!");
  CHECK(ecs.greet(LanguageCode::ES) == "¡Hola Tests!");
  CHECK(ecs.greet(LanguageCode::FR) == "Bonjour Tests!");
}

TEST_CASE("SubzeroECS version") {
  static_assert(std::string_view(SubzeroECS_VERSION) == std::string_view("1.0"));
  CHECK(std::string(SubzeroECS_VERSION) == std::string("1.0"));
}
