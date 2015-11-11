#include "TreeNode.h"
#include "TreeSegment.h"

namespace octet {

	class Tree
	{
		int framesPerStep = 0;

		float segmentLength = 1;
		float segmentThickness = 0.05f;

		ref<visual_scene> scene;

		TreeNode rootNode;
		TreeNode *currentNode;
		std::vector<TreeNode> *memoryNode;
		std::vector<float> *memoryAngle;
		std::vector<float> *memoryAngleY;

		std::vector<mesh_cylinder> *meshes;
		std::vector<TreeSegment> *segments;
		std::vector<TreeNode> *nodes;

		std::vector<Tree> *treesArray;

		vec4 currentColor;

		float angle = 20;
		float angleY = 0;
		float currentRot;
		float currentRotY;

		std::string dividedDescription;

		int k; //index for the loop
		int* idGenerator;
		int id;

	public:
		bool enabled = false;		

		void init(mat4t _root, ref<visual_scene> _scene, std::vector<Tree> *t, std::vector<mesh_cylinder> *m, std::string description, int *idGen, int framesPStep)
		{
			id = *idGen;
			//*idGen++;
			idGenerator = idGen;
			enabled = true;

			framesPerStep = framesPStep;

			treesArray = t;

			rootNode = *(new TreeNode());
			rootNode.parent = nullptr;
			rootNode.transform = _root;

			memoryAngle = new std::vector<float>();
			memoryAngleY = new std::vector<float>();
			memoryNode = new std::vector<TreeNode>();

			segments = new std::vector<TreeSegment>();
			nodes = new std::vector<TreeNode>();		

			meshes = m;

			scene = _scene;

			currentColor = vec4(1, 0, 0, 1);

			currentNode = &rootNode;
			currentRot = 0;
			currentRotY = 0;
			
			dividedDescription = description;

			k = 0;
		}

		void setSegmentLength(float l)
		{
			segmentLength = l;
		}

		void setSegmentThickness(float t)
		{
			segmentThickness = t;
		}

		void setAngle(float a)
		{
			angle = a;
		}

		void setAngleY(float y)
		{
			angleY = y;
		}

		void setCurrentRotZ(float z)
		{
			currentRot = z;
		}
		void setCurrentRotY(float y)
		{
			currentRotY = y;
		}

		void GrowSegments()
		{
			//make the segments grow
			for (int j = 0; j < segments->size(); j++)
			{
				TreeSegment segment = (*segments)[j];
				(*segments)[j].Grow();
			}
		}


		void Grow()
		{		
			const char* s = dividedDescription.c_str();

			int aux = nodes->size();
			if (aux > 0)
			{
				currentNode = &(*nodes)[aux - 1];
			}

			while (k < strlen(s))
			{
				char command = dividedDescription[k];
				if (command == 'F') //create segment
				{					
					//create a segment from current node 
					TreeSegment newSegment;			
					newSegment.thickness = segmentThickness;
					newSegment.lenght = segmentLength;
					newSegment.startNode = currentNode;
					newSegment.color = currentColor;
					newSegment.rotZ = currentRot;
					newSegment.rotY = currentRotY;
					

					//draw segment
					TreeNode n = newSegment.Init(scene, meshes, framesPerStep);

					//push created node and assign to segment					
					nodes->push_back(n);
					
					newSegment.endNode = &n;

					//push it to the segments array					
					segments->push_back(newSegment);

					//advance currentNode to the end of the new segment
					currentNode = &n;
					k++;					

					return; //break the execution until next step!
				}
				else if (command == '[') //create a new tree from the current node
				{
					/*TreeNode n = *currentNode;					
					memoryNode->push_back(n);

					float a = currentRot;
					memoryAngle->push_back(a);

					float y = currentRotY;
					memoryAngleY->push_back(y);*/

					std::string str = dividedDescription;
					
					int beginPos = k+1;
					int endPos;

					//find which closing bracket we need, counting the open brackets
					//int openBracketOcurrences = std::count(str.begin(), str.end(), '[');
					bool found = false;
					int openBrackets = 0;
					for (int i = beginPos; found == false; i++)
					{
						char c = dividedDescription[i];
						if (c == '[')
						{
							openBrackets++;
						}
						else if (c == ']')
						{
							if (openBrackets == 0)
							{
								found = true;
								endPos = i;
							}
							else
							{
								openBrackets--;
							}
						}
					}					

					str = str.substr(beginPos, endPos-beginPos);
					dividedDescription.erase(beginPos-1, endPos-beginPos+2);

					int newId = *(idGenerator)+1;		
					*(idGenerator) += 1;
					Tree *tree = &(*treesArray)[newId];
					tree->enabled = true;
					tree->init(currentNode->transform, scene, treesArray, meshes, str, idGenerator, framesPerStep);
					tree->setAngle(angle);
					tree->setAngleY(angleY);
					tree->setSegmentLength(segmentLength);
					tree->setSegmentThickness(segmentThickness);
					tree->setCurrentRotY(currentRotY);
					tree->setCurrentRotZ(currentRot);

					//treesArray->push_back(tree);

					k--; //we would jump over the next command as collateral result of the string deletion
				}
				else if (command == ']') //load node from memory
				{
						/*int aux = memoryNode->size();
						currentNode = &(*memoryNode)[aux-1];
						memoryNode->pop_back();

						aux = memoryAngle->size();
						currentRot = (*memoryAngle)[aux - 1];
						memoryAngle->pop_back();

						aux = memoryAngleY->size();
						currentRotY = (*memoryAngleY)[aux - 1];
						memoryAngleY->pop_back();*/

				}				
				else if (command == 'C') //change color (looking for next character)
				{
					if (dividedDescription[k+1] == '0')
					{
						currentColor = vec4(1, 0, 0, 1);
					}
					else if (dividedDescription[k + 1] == '1')
					{
						currentColor = vec4(0, 1, 0, 1);
					}
					else if (dividedDescription[k + 1] == '2')
					{
						currentColor = vec4(0, 0, 1, 1);
					}
				}
				else if (command == '-') //rotate -
				{
					currentRot -= angle;
				}
				else if (command == '+') //rotate +
				{
					currentRot += angle;
				}
				else if (command == '*') //rotate + Y
				{
					currentRotY += angleY;
				}
				else if (command == '/') //rotate - Y
				{
					currentRotY -= angleY;
				}
				k++;
			}
			
		}
	};
}