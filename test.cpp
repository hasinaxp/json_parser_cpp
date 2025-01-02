#include "json_parser.hpp"


void test_json_parse()
{
    //simple object
    const char *json_str = "{\"name\": \"John\", \"age\": 30, \"car\": null}";
    auto json = sp::parse_json(json_str);
    std::cout << sp::to_string(json, false, 2) << std::endl;
    //simple array
    json_str = "[\"apple\", \"banana\", \"cherry\", 1, 2, true, false, null]";
    json = sp::parse_json(json_str);
    std::cout << sp::to_string(json, false, 2) << std::endl;
    //nested object
    json_str = "{\"name\": \"John\", \"age\": 30, \"car\": {\"make\": \"Toyota\", \"model\": \"Corolla\"}}";
    json = sp::parse_json(json_str);
    std::cout << sp::to_string(json, false, 2) << std::endl;
    //nested array
    json_str = "[\"apple\", \"banana\", \"cherry\", [\"apple\", \"banana\", \"cherry\"], 1, 2, true, false, null]";
    json = sp::parse_json(json_str);
    std::cout << sp::to_string(json, false, 2) << std::endl;
    //complex object
    json_str = "{\"name\": \"John\", \"age\": 30, \"car\": {\"make\": \"Toyota\", \"model\": \"Corolla\"}, \"children\": [{\"name\": \"Alice\", \"age\": 5}, {\"name\": \"Bob\", \"age\": 7}]}";
    json = sp::parse_json(json_str);
    std::cout << sp::to_string(json, false, 2) << std::endl;
    //complex array
    json_str = "[\"apple\", \"banana\", \"cherry\", [\"apple\", \"banana\", \"cherry\"], 1, 2, true, false, null, [{\"name\": \"Alice\", \"age\": 5}, {\"name\": \"Bob\", \"age\": 7}]]";
    json = sp::parse_json(json_str);
    std::cout << sp::to_string(json, false, 2) << std::endl;
    //empty object
    json_str = "{}";
    json = sp::parse_json(json_str);
    std::cout << sp::to_string(json, false, 2) << std::endl;
    //highly complex json
    json_str = "{\"name\": \"John\", \"age\": 30, \"car\": {\"make\": \"Toyota\", \"model\": \"Corolla\"}, \"children\": [{\"name\": \"Alice\", \"age\": 5}, {\"name\": \"Bob\", \"age\": 7}], \"friends\": [{\"name\": \"Alice\", \"age\": 5, \"children\": [{\"name\": \"Alice\", \"age\": 5}, {\"name\": \"Bob\", \"age\": 7}]}, {\"name\": \"Bob\", \"age\": 7, \"children\": [{\"name\": \"Alice\", \"age\": 5}, {\"name\": \"Bob\", \"age\": 7}]}]}";
    json = sp::parse_json(json_str);
    std::cout << sp::to_string(json, false, 2) << std::endl;
}


int main(int argc, char ** argv)
{
	test_json_parse();
}
