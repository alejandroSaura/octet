#include "TreeNode.h"
#include "TreeSegment.h"

namespace octet {

	class Tree
	{
		float segmentLength = 1;
		float segmentThickness = 0.05f;

		ref<visual_scene> scene;

		TreeNode *rootNode;
		TreeNode *currentNode;
		dynarray<TreeNode> memoryNode;
		dynarray<float> memoryAngle;

		dynarray<TreeSegment> segments;
		dynarray<TreeNode> nodes;

		vec4 currentColor;

		float angle = 20;
		float currentRot;

	public:
		Tree(mat4t *_root, ref<visual_scene> _scene)
		{
			rootNode = new TreeNode();
			rootNode->parent = nullptr;
			rootNode->transform = *_root;

			memoryAngle = *new dynarray<float>(500);
			memoryNode = *new dynarray<TreeNode>(500);

			segments = *new dynarray<TreeSegment>(5000);
			nodes = *new dynarray<TreeNode>(5000);

			scene = _scene;

			currentColor = vec4(1, 0, 0, 1);
			
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

		void Update(std::string description)
		{
			const char *dividedDescription = description.c_str();			

			currentNode = rootNode;
			currentRot = 0;
			for (int k = 0; k < strlen(dividedDescription); k++)
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

					//draw segment
					TreeNode n = newSegment.Init(scene);

					//push created node and assign to segment					
					nodes.push_back(n);
					
					newSegment.endNode = &n;

					//push it to the segments array					
					//segments.push_back(newSegment);

					//advance currentNode to the end of the new segment
					currentNode = newSegment.endNode;
				}
				else if (command == '[') //save node in memory
				{
					TreeNode n = *currentNode;
					float a = currentRot;
					memoryNode.push_back(n);
					memoryAngle.push_back(a);
				}
				else if (command == ']') //load node from memory
				{
						int aux = memoryNode.size();
						currentNode = &memoryNode[aux-1];
						memoryNode.pop_back();

						aux = memoryAngle.size();
						currentRot = memoryAngle[aux - 1];
						memoryAngle.pop_back();

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
			}
		}
	};
}