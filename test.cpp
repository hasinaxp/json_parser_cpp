#include "json_parser.hpp"


int main(int argc, char ** argv)
{
	//demo creating an json object
	sp::JSONNode json = sp::JSONNode::JSONObjectNode();
	json.pushProperty("string", sp::JSONNode::JSONStringNode("value1"));
	
	//json object property indexed access
	// this returns the sp::JSONNode referance according to the key
	json["number"] = sp::JSONNode::JSONNumberNode(100);
	json["boolean"] = sp::JSONNode::JSONBooleanNode(true);
	json["object"] = sp::JSONNode::JSONObjectNode();
	json["object"]["array"] = sp::JSONNode::JSONArrayNode();
	json["object"]["nested"] = sp::JSONNode::JSONObjectNode();
	json["object"]["nested"]["key"] = sp::JSONNode::JSONStringNode("value");
	for (int i = 0; i < 4; i++)
	{
		json["object"]["array"].push(sp::JSONNode::JSONNumberNode(i));
	}

	std::cout << "number: " << json["object"]["array"][1].getInt() << std::endl;
	
	//json array item indexed access 
	json["object"]["array"][3] = sp::JSONNode::JSONNumberNode(2.2);

	// demo getting the string version of the json object
	std::cout << json.toString() << std::endl;

	//demo reading json from file	
	json.readFromFile("demo.json");

	//demo logging a json
	sp::PrintJSONNode(json["keyObject"]);
}
