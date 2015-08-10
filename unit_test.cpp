#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <regex>

#include "NcrgMapping.hpp"

using namespace std;
using namespace ncrg;

TEST_CASE("NcrgMapping correctly parses simple regex", "[NcrgMapping]" ) {

	string regex = "(<EMPLOYEE_NAME>[a-zA-Z]+)\\|(<EMPLOYEE_INCOME>[0-9$]+)\\|(<EMPLOYEE_CITY>[a-zA-Z]+)";
	string data = "Alex|50000|London";	
	ncrg::NcrgMapping<char> mapping(regex);	
	std::smatch match;
	bool result = std::regex_match(data, match, mapping.getStdRegex());	
	auto container = mapping.getValuesForMatch(match);
	REQUIRE(result);
	REQUIRE(container[0].first == "EMPLOYEE_NAME");
	REQUIRE(container[0].second == "Alex");
	REQUIRE(container[1].first == "EMPLOYEE_INCOME");
	REQUIRE(container[1].second == "50000");
	REQUIRE(container[2].first == "EMPLOYEE_CITY");
	REQUIRE(container[2].second == "London");
}