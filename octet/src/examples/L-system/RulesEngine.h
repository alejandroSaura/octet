#include <string>

namespace octet {


	class RulesEngine {

		struct RightSide
		{
			string st;
			float weight;
			float probability;
		};

		struct rule
		{
			string leftSide;
			dynarray<RightSide> *rightSides;
		};

		void calculateProbabilities(dynarray<RightSide> &array)
		{
			float totalWeight = 0;
			for (int i = 0; i < array.size(); i++)
			{
				totalWeight += array[i].weight;
			}

			float lastProbability = 0;
			for (int i = 0; i < array.size(); i++)
			{
				float p = array[i].weight/totalWeight;
				array[i].probability = lastProbability + p;
				lastProbability += p;
			}
		}

		string state;
		dynarray<rule> rules;
		int iterationCounter = 0;

		//auxiliar function to replace string patterns
		void replaceAll(string& s, const string& f, const string& t) {

			std::string str = std::string(s.c_str());
			std::string from = std::string(f.c_str());
			std::string to = std::string(t.c_str());

			if (from.empty())
				return;
			size_t start_pos = 0;
			while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
				str.replace(start_pos, from.length(), to);
				start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
			}
			s = string(str.c_str());
		}

	public:

		//set the start point for the engine
		void setAxiom(const char *a)
		{
			if (iterationCounter == 0)
			{
				state = string(a);
			}
			else
			{
				return;
			}
		}

		void addRule(const char *left, const char *right, float weight)
		{
			for (int i = 0; i < rules.size(); i++)
			{
				if (rules[i].leftSide.operator==(string(left)))
				{
					RightSide rs;
					rs.st = string(right);
					rs.weight = weight;
					rs.probability = 0;

					rules[i].rightSides->push_back(rs);
					calculateProbabilities(*(rules[i].rightSides));
					return;
				}
			}			
			//if we reached this point, there's no other rule with same left part
			rule newRule;
			newRule.leftSide = left;

			//dynarray<RightSide> a = new dynarray<RightSide>();
			newRule.rightSides = new dynarray<RightSide>();

			RightSide newRight;
			newRight.st = right;
			newRight.weight = weight;
			newRight.probability = 1;

			newRule.rightSides->push_back(newRight);
			rules.push_back(newRule);
		}

		std::string iterate()
		{
			const char *dividedState = state.c_str();
			dynarray<string> result;

			bool ruleApplied = false;

			for (int k = 0; k < strlen(dividedState); k++)
			{
				char aux2 = dividedState[k];

				ruleApplied = false;

				//for each character of the state string
				for (int i = 0; i < rules.size(); i++)
				{
					char aux1 = rules[i].leftSide.c_str()[0];
					//check if we can apply each rule (if the left side of any rule is this concrete character)
					if (rules[i].leftSide.c_str()[0] == dividedState[k])
					{
						//TO-DO: maybe we could add here some more conditions, as check the neighbours

						//Select one of the posible right sides of the rules
						float random = 0.9f; //TO-DO: make it random!
						int j = 0;
						for (j = 0; j < (rules[i].rightSides)->size(); j++)
						{
							if (((rules[i].rightSides)->operator[](j)).probability > random)
							{
								break;
							}
						}
						RightSide s = (rules[i].rightSides)->operator[](j);

						//Put this right side instead of the character
						result.push_back(s.st);
						ruleApplied = true;
					}					
				}
				if (ruleApplied == false)
				{
					//No rule applied - add the character to the result as it is.
					std::string st{ dividedState[k] };
					result.push_back(string(st.c_str()));
				}
			}

			state = "";
			for (int i = 0; i < result.size(); i++)
			{
				state += result[i].c_str();
			}

			iterationCounter++;
			return state;
		}
	};

}

