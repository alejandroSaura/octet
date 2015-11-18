////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
	/// Scene containing a box with octet.
	class Lsystem : public app {
		// scene for drawing box
		ref<visual_scene> app_scene;
		RulesEngine rulesEngine;

		ref<camera_instance> the_camera;
		vec3 cameraPos;
		mouse_look mouse_look_helper;


		std::vector<Tree> *trees;
		Tree tree;

		std::string treeDescription;

		int counter = 0;
		int framesPerStep = 4;

		//std::vector<mesh_cylinder> *cylinderMeshes;
		MeshPool *cylMeshPool;

		int idGen = 1; //clear it at scene redraw

		//treeParameters
		float segmentLength;
		float segmentThickness;
		float framesUntilFinalColor;
		float rotZ;
		float rotY;

		//ruleEngine parameters
		struct rule
		{
			std::string leftSide;
			std::string rightSide;
			float probability;
		};
		struct treeDefinition
		{
			std::string axiom;
			std::vector<rule> rules;
		};
		//we'll create a vector of rules for each tree type defined on the csv
		std::vector<treeDefinition> treesDefinition;

		bool freeMode = false;

		int currentTree = 0;
		int currentIteration = 1;

		int maxIteration = 5;

	public:


		/// this is called when we construct the class before everything is initialised.
		Lsystem(int argc, char **argv) : app(argc, argv) {
		}

		/// this is called once OpenGL is initialized
		void app_init() {


			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			the_camera = app_scene->get_camera_instance(0);
			the_camera->set_far_plane(100000);

			scene_node *camera_node = the_camera->get_node();
			mat4t &camera_to_world = camera_node->access_nodeToParent();

			mouse_look_helper.init(this, 200.0f / 360.0f, false);

			cylMeshPool = new MeshPool();

			ReadConfFile();

			currentTree = 0;
			currentIteration = 1;
			createTree(currentTree, currentIteration);


			app_scene->set_render_debug_lines(true);
		}

		void ReadConfFile()
		{
			// store the line here
			char buffer[2048];			//memory to put lines
			dynarray<string> values;			//memory to store values from csv

			std::ifstream pipe("config.csv");  //connection to file line by line stream

			if (pipe.bad() == false)			// bad = error in the pipe e.g. file don't exist
			{
				int rowCounter = 0;
				// loop over lines
				while (!pipe.eof())				//read until end of the file
				{
					pipe.getline(buffer, sizeof(buffer));			//read a row
					if (buffer[0] == '\0')
					{
						continue; //if the line is empty, skip
					}

					if (rowCounter == 1) //parameters row
					{
						// loop over columns
						char *columnPointer;					//Pointer - read colum
						columnPointer = &(buffer[0]);			// pointing to first char of each cell
						for (int col = 0;; col++)				// read how many col in row
						{
							char *characterPointer;
							characterPointer = columnPointer;

							float value;
							while (*characterPointer != 0 && *characterPointer != ',')
							{
								++characterPointer;
							}
							if (*characterPointer == 0) break; //end of row

							const char *c = columnPointer;
							unsigned int size = characterPointer - columnPointer;
							if (size != 0)
							{								
								value = std::stof(std::string(c, size));
							}
							else
							{
								value = 0;
							}							

							switch (col)
							{
							case 0: //segmentLength
								segmentLength = value;
								break;
							case 1: //segmentThickness
								segmentThickness = value;
								break;
							case 2: //framesUntilFinalColor
								framesUntilFinalColor = value;
								break;
							case 3: //rotZ
								rotZ = value;
								break;
							case 4: //rotY
								rotY = value;
								break;
							}							

							columnPointer = characterPointer + 1;
						}
					}

					if (rowCounter >= 4) //rules definition. One set (tree) per row
					{
					treeDefinition treeDef; //rules and axiom for this tree
					std::vector<std::string> rawData; //storage of all columns

						// loop over columns
						char *columnPointer;					//Pointer - read colum
						columnPointer = &(buffer[0]);			// pointing to first char of each cell
						for (int col = 0;; col++)				// read how many col in row
						{
							char *characterPointer;
							characterPointer = columnPointer;

							std::string value;
							while (*characterPointer != 0 && *characterPointer != ',')
							{								
								++characterPointer;
							}
							if (*characterPointer == 0) break; //end of row

							const char *c = columnPointer;
							unsigned int size = characterPointer - columnPointer;
							if (size != 0)
							{
								value = std::string(c, size);
							}
							else
							{
								value = "";
							}			

							if (col == 0) //axiom
							{
								treeDef.axiom = value;
							}
							else
							{
								rawData.push_back(value);
							}

							columnPointer = characterPointer + 1;
						}

						//sort the rawData into rules
						for (int k=0; k < rawData.size(); k++)
						{
							if (k % 2 == 0 && rawData[k].size()!=0) //rule string
							{
								rule r;
								r.leftSide = rawData[k].substr(0, rawData[k].find("=", 0));
								r.rightSide = rawData[k].substr(rawData[k].find("=", 0)+1, rawData[0].size());
								r.probability = std::stof(rawData[k + 1]);
								treeDef.rules.push_back(r);
							}
						}

						treesDefinition.push_back(treeDef);
					}					
					rowCounter++;
				}
			}


			//rulesEngine.setAxiom("FX");
			//rulesEngine.addRule("F", "C0F/F-[C1-F+F]+[C2+F-F]", 1);
			//rulesEngine.addRule("X", "C0F*F++[C1+F/F]+[C2-FF]", 0.30f);
			//rulesEngine.addRule("X", "C0F**F", 0.70f);
			////rulesEngine.addRule("]", "L]", 1); //add intermediate leaves

			//////rulesEngine.addRule("FF", "F*", 0.5f);

			////basic single branch probability example
			///*rulesEngine.setAxiom("F");
			//rulesEngine.addRule("F", "F+F", 0.6f);
			//rulesEngine.addRule("F", "F-F", 0.4f);*/

			//std::string result1 = rulesEngine.iterate();
			//std::string result2 = rulesEngine.iterate();
			//std::string result3 = rulesEngine.iterate();
			//std::string result4 = rulesEngine.iterate();
			///*std::string result5 = rulesEngine.iterate();
			//std::string result6 = rulesEngine.iterate();*/
			//rulesEngine.clearRules();
			//rulesEngine.addRule("]", "L]", 1); //add end leaves
			//std::string result5 = rulesEngine.iterate();
			//treeDescription = result5;
		}

		void createTree(int treeId, int iteration)
		{
			freeMode = false;

			//cleanUp
			app_scene->reset();
			//trees->clear();
			idGen = 1;
			app_scene->create_default_camera_and_lights();
			trees = new std::vector<Tree>(4000);

			//RuleEngine start
			rulesEngine.clearRules();
			rulesEngine.clearState();
			rulesEngine.setAxiom(treesDefinition[treeId].axiom.c_str());
			int size = treesDefinition[treeId].rules.size();
			for (int i = 0; i < size; i++)
			{
				rulesEngine.addRule(
					treesDefinition[treeId].rules[i].leftSide.c_str(),
					treesDefinition[treeId].rules[i].rightSide.c_str(),
					treesDefinition[treeId].rules[i].probability);
			}
			//iterate
			std::string result;
			for (int i = 0; i < iteration; i++)
			{
				result = rulesEngine.iterate();
			}
			//add leafs
			rulesEngine.clearRules();
			rulesEngine.addRule("]", "L]", 1);
			result = rulesEngine.iterate();


			mat4t root;
			root.loadIdentity();

			tree.init(root, app_scene, trees, cylMeshPool, result, &idGen, framesPerStep);
			tree.setAngle(-25);
			tree.setAngleY(20);
			tree.setSegmentLength(0.5f);
			tree.setSegmentThickness(0.005f);
			tree.setInitialColor(vec4(0.50196078431f, 0.50196078431f, 0, 0));
			tree.setFinalColor(vec4(0.271f, 0.192f, 0.047f, 0));
			tree.setFramesUntilFinalColor(25);

			trees->push_back(tree);
		}

		void draw_world(int x, int y, int w, int h) {

			if (!freeMode) //tree and iteration change
			{
				if (is_key_going_down(key_left))
				{
					currentTree -= 1;
					currentIteration = 1;
					if (currentTree < 0)
					{
						currentTree = 0;
						currentIteration = 1;
					}
					else
					{
						createTree(currentTree, currentIteration);
					}					
					printf("currentTree: %d\n", currentTree);
					printf("currentIteration: %d\n", currentIteration);
					printf("\n");
				}
				if (is_key_going_down(key_right))
				{
					currentTree += 1;
					currentIteration = 1;
					if (currentTree > treesDefinition.size() - 1)
					{
						currentTree = treesDefinition.size() - 1;						
					}
					else
					{
						createTree(currentTree, currentIteration);
					}					
					printf("currentTree: %d\n", currentTree);
					printf("currentIteration: %d\n", currentIteration);
					printf("\n");
				}
				if (is_key_going_down(key_up))
				{
					currentIteration += 1;
					if (currentIteration > maxIteration)
					{
						currentIteration = maxIteration;
					}
					else
					{
						createTree(currentTree, currentIteration);
					}		
					printf("currentTree: %d\n", currentTree);
					printf("currentIteration: %d\n", currentIteration);
					printf("\n");
				}
				if (is_key_going_down(key_down))
				{
					currentIteration -= 1;
					if (currentIteration < 1)
					{
						currentIteration = 1;						
					}
					else
					{
						createTree(currentTree, currentIteration);
					}
					printf("currentTree: %d\n", currentTree);
					printf("currentIteration: %d\n", currentIteration);
					printf("\n");

				}
			}

			//if (is_key_down(key_alt))
			{
				int max = trees->size();

				//segment growing			
				for (int i = 0; i < max; i++)
				{
					if ((*trees)[i].enabled)
						(*trees)[i].GrowSegments();
				}
				//new segments creation
				counter++;
				if (counter > framesPerStep)
				{
					for (int i = 0; i < max; i++)
					{
						if ((*trees)[i].enabled)
							(*trees)[i].Grow();
					}
					counter = 0;
				}
			}

			scene_node *camera_node = the_camera->get_node();
			mat4t &camera_to_world = camera_node->access_nodeToParent();

			if (freeMode) //camera movement
			{
				if (is_key_down(key_left))
				{
					camera_node->translate(vec3(-0.15f, 0, 0));
				}
				if (is_key_down(key_right))
				{
					camera_node->translate(vec3(0.15f, 0, 0));
				}
				if (is_key_down(key_up))
				{
					camera_node->translate(vec3(0, 0, -0.15f));
				}
				if (is_key_down(key_down))
				{
					camera_node->translate(vec3(0, 0, 0.15f));
				}
			}

			if (is_key_going_down(key_f1)) //free mode
			{
				//printf("freeMode toggle");
				if(freeMode)
				{
					freeMode = false;
				}
				else
				{
					set_absolute_mouse_movement(0, 0);
					freeMode = true;
				}
				
			}
			if (is_key_down(key_backspace))
			{				
				createTree(0, 3);
			}
			if (is_key_down(key_ctrl))
			{
				camera_node->translate(vec3(0, -0.15f, 0));
			}
			if (is_key_down(key_shift))
			{
				camera_node->translate(vec3(0, 0.15f, 0));
			}

			if (freeMode)
			{
				mouse_look_helper.update(camera_to_world);
			}
			else
			{
				//move the camera to fit the tree size
				the_camera = app_scene->get_camera_instance(0);

				aabb bb = app_scene->get_world_aabb();
				bb = bb.get_union(aabb(vec3(0, 0, 0), vec3(5, 5, 5)));
				float bb_size = length(bb.get_half_extent()) * 2.0f;
				float distance = app_scene->max(bb.get_max().z(), bb_size) * 0.9f;
				vec3 targetPos = vec3(0, bb.get_center().y() + 3, distance);
				camera_node->loadIdentity();

				cameraPos = lerp(cameraPos, targetPos, 0.033333f * 5);
				camera_node->translate(cameraPos);
			}

			


			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);

		}

		vec3 lerp(vec3 a, vec3 b, float t){
			return (1 - t)*a + t*b;
		}

	};
}
