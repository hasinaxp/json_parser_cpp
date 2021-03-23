#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>

/*



*/

//TODO : implement error check

namespace sp
{

	const std::string JSONFIELD_TEXT_FILTER_SEQUENCE = "~iii(>,<)";
	const std::string JSONFIELD_ARRAY_ENTRY_SEQUENCE = "~iii(>_<)";

	//
	//types:
	//k -> key
	//o -> object
	//a -> array
	//u -> unknown
	//b -> boolean
	//s ->string
	//n -> number



	class JSONNode
	{
	public:
		std::string _value;
		char _type;
		std::vector<JSONNode> _children;
		static JSONNode ERROR;
		JSONNode()
			:_value(""),
			_type('u'),
			_children({})
		{
		}
		JSONNode(std::string value, char type = 's', std::vector<JSONNode> children = {})
			:_value(value),
			_type(type),
			_children(children)
		{
		}

		void readFromString(std::string str)
		{
			this->_value = "root";

			// read file as tokens
			std::vector<std::string> tokens = {};
			tokens = tokenize(str);
	
			// use tokens to fill tree
			JSONNode* current = this;
			std::vector<JSONNode*> nodeStack = {};
			std::vector<char> typeStack = {};
			std::string temp("");
			char type = 'u';

			for (std::string token : tokens)
			{
				if (token == "{")
				{
					current->_type = 'o';
					JSONNode node;
					current->_children.push_back(node);
					nodeStack.push_back(current);
					typeStack.push_back('o');
					current = &(current->_children[0]);
				}
				else if (token == "}")
				{
					if (typeStack.back() == 'k')
					{
						current->_value = temp;
						current->_type = type;
						typeStack.pop_back();
						typeStack.pop_back();
						current = nodeStack.back();
						nodeStack.pop_back();
					}
				}
				else if (token == "[")
				{
					current->_type = 'a';
					JSONNode node;
					current->_children.push_back(node);
					nodeStack.push_back(current);
					typeStack.push_back('a');
					current = &(current->_children[0]);
				}
				else if ( token == "]")
				{
						if (current->_type != 'a' && current->_type != 'o')
						{
							current->_value = temp;
							current->_type = type;
						}
						typeStack.pop_back();
						typeStack.pop_back();
						nodeStack.pop_back();
						current = nodeStack.back();
						if (current->_type == 'a')
						{
							nodeStack.pop_back();
							current = nodeStack.back();
						}

				}
				else if (token == ":")
				{
					current->_type = 'k';
					current->_value = temp;
					JSONNode node;
					current->_children.push_back(node);
					typeStack.push_back('k');
					current = &(current->_children.back());
				}
				else if (token == ",")
				{
					if (typeStack.back() == 'k')
					{
					
						if (current->_type != 'a' && current->_type != 'o')
						{
							current->_value = temp;
							current->_type = type;
						}
						current = nodeStack.back();
						typeStack.pop_back();
						JSONNode node;
						current->_children.push_back(node);
						current = &(current->_children.back());
					}
					else if (typeStack.back() == 'o')
					{
						current = nodeStack.back();
						JSONNode node;
						current->_children.push_back(node);
						current = &(current->_children.back());
					}
					else if (typeStack.back() == 'a')
					{
						
						if (current->_type != 'a' && current->_type != 'o')
						{
							current->_value = temp;
							current->_type = type;
						}
						current = nodeStack.back();
						JSONNode node;
						current->_children.push_back(node);
						current = &(current->_children.back());
					}
				}
				else
				{
					type = 'u';
					if (token.length() > JSONFIELD_TEXT_FILTER_SEQUENCE.length())
					{
						uint32_t len = JSONFIELD_TEXT_FILTER_SEQUENCE.length();
						if (token.substr(0, len) == JSONFIELD_TEXT_FILTER_SEQUENCE)
						{
							type = 's';
							token = token.substr(len);
						}
					}
					else if (type == 'u' && (token == "true" || token == "false"))
						type = 'b';
					else
						type = 'n';

					temp = token;
				}
			}
		}


		void readFromFile(std::string filepath)
		{
			std::ifstream file(filepath);
			if (file.is_open())
			{
				std::stringstream ss;
				ss << file.rdbuf();
				readFromString(ss.str());
			}
		}


		//convert JSONNode to jsonString
		std::string toString()
		{
			std::string output = "";
			if (this->_type == 'k')
			{
				output += "\"";
				output += this->_value;
				output += "\":";
				output += this->_children[0].toString();
				output += ",";
			}
			else if (this->_type == 'o')
			{
				output += "{";
				for (JSONNode& prop : this->_children)
				{
					output += prop.toString();
				}
				output.pop_back(); // remove last ","
				output += "}";
			}
			else if (this->_type == 'a')
			{
				output += "[";
				for (JSONNode& item : this->_children)
				{
					output += item.toString();
					output += ",";
				}
				output.pop_back(); // remove last ","
				output += "]";
			}
			else if (this->_type == 's')
			{
				output += "\"";
				output += this->_value;
				output += "\"";
			}
			else if (this->_type == 'n' || this->_type == 'b')
			{
				output += this->_value;
			}

			return output;

		}

		//json object node random access
		JSONNode& operator [](std::string key)
		{
			//only objects are allowed
			assert(this->_type == 'o');
			int i;

			for (i = 0; i < _children.size(); i++)
			{
				if (this->_children[i]._value == key)
				{
					//std::cout << "index found" << std::endl;
					break;
				}
			}
			if (i == _children.size())
			{
				JSONNode node(key, 'k', {JSONNode()});
				this->_children.push_back(node);
				return this->_children.back()._children[0];
			}
			return _children[i]._children[0];
		}
		

		//json arraynode random access 
		JSONNode& operator [](uint32_t index)
		{
			//only arrays are allowed
			assert(this->_type == 'a');
			if(index < this->_children.size())
				return this->_children[index];
			else
			{
				return ERROR;
			}
		}
		
		std::string get()
		{
			return this->_value;
		}
		int getInt()
		{
			return std::stoi(this->_value);
		}
		float getFloat()
		{
			return std::stof(this->_value);
		}
		bool getBool()
		{
			return (this->_value == "false" ? false : true);
		}


		void push(JSONNode node)
		{
			assert(this->_type == 'a');
			this->_children.push_back(node);
		}

		void pushProperty(std::string key, JSONNode node)
		{
			assert(this->_type == 'o');
			(*this)[key] = node;
		}

		static JSONNode JSONObjectNode()
		{
			JSONNode node("", 'o');
			return node;
		}

		static JSONNode JSONArrayNode()
		{
			JSONNode node("", 'a');
			return node;
		}
		static JSONNode JSONStringNode(std::string str)
		{
			JSONNode node(str, 's');
			return node;
		}
		static JSONNode JSONNumberNode(float num)
		{
			JSONNode node(std::to_string(num), 'n');
			return node;
		}
		static JSONNode JSONBooleanNode(bool boolean)
		{
			if (boolean)
				return JSONNode("true", 'b');
			else
				return JSONNode("false", 'b');
		}

	private:

		std::vector<std::string> tokenize(std::string str)
		{
			std::vector<std::string> tokens = {};
			bool pushWhiteSpace = false;
			std::string temp("");
			for (char c : str)
			{
				switch (c)
				{
				case '[':
				{
					tokens.push_back("[");
				}break;
				case ']':
				{
					if (temp != "") tokens.push_back(temp);
					tokens.push_back("]");
				}break;
				case '{':
				{
					tokens.push_back("{");
				}break;
				case '}':
				{
					if (temp != "") tokens.push_back(temp);
					tokens.push_back("}");
				}break;
				case ':':
				{
					if (temp != "") tokens.push_back(temp);
					tokens.push_back(":");
					temp = "";
				}break;
				case ',':
				{
					if (temp != "") tokens.push_back(temp);
					tokens.push_back(",");
					temp = "";
				}break;
				default:
				{
					if (c == '\n') continue;
					if (c == '\"')
					{
						pushWhiteSpace = !pushWhiteSpace;
						if (pushWhiteSpace) temp += JSONFIELD_TEXT_FILTER_SEQUENCE;
						continue;
					}
					if ((!pushWhiteSpace) && c == ' ') continue;
					temp += c;
				}break;
				}
			}
			return tokens;
		}

		std::vector<std::string> tokenizeQuery(std::string& str)
		{
			std::vector<std::string> tokens;
			std::string token = "";
			bool pushLast = true;
			for (auto c : str)
			{
				if (c == '.')
				{
					tokens.push_back(token);
					token = "";
					pushLast = true;
				}
				else if (c == '[')
				{
					if (token.length() > 0)
						tokens.push_back(token);
					tokens.push_back(JSONFIELD_ARRAY_ENTRY_SEQUENCE);
					token = "";
				
				}
				else if (c == ']')
				{
					tokens.push_back(token);
					pushLast = false;
				}
				else
				{
					token += c;
				}
			}

			if (token.length() > 0 && pushLast) 
				tokens.push_back(token);


			return tokens;
		}

	};

	JSONNode JSONNode::ERROR = JSONNode("error", 'u', {});

	void PrintJSONNode(const JSONNode& tree, int level = 0)
	{
		for (int i = 0; i < level; i++)
			std::cout << "    ";
		std::cout << "-" << tree._value << "( " << tree._type << " )" << std::endl;

		for (JSONNode child : tree._children)
			PrintJSONNode(child, level + 1);
	}

}


