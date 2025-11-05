#include "TestTypes.hpp"

#include "CppUnitTest.hpp"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template<> std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<Human>(const Human& inputValue)
{
	return L"human";
}



namespace SubzeroECS {
namespace Test 
{
} //END: Test
} //END: SubzeroECS