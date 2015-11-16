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

	mouse_look mouse_look_helper;
	helper_fps_controller fps_helper;
	ref<scene_node> player_node;
	vec3 cameraPos;

	std::vector<Tree> *trees;
	Tree tree;

	std::string treeDescription;

	int counter = 0;
	int framesPerStep = 4;

	std::vector<mesh_cylinder> *meshes;

	int idGen = 1;

  public:
	

    /// this is called when we construct the class before everything is initialised.
    Lsystem(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {

	meshes = new std::vector<mesh_cylinder>();

      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  the_camera = app_scene->get_camera_instance(0);
	  the_camera->set_far_plane(10000);

	  scene_node *camera_node = the_camera->get_node();
	  mat4t &camera_to_world = camera_node->access_nodeToParent();
	  //camera_node->translate(vec3(0, 0, 1));

	  //mouse_look_helper.init(this, 200.0f / 360.0f, false);
	  //fps_helper.init(this);       

	  rulesEngine.setAxiom("FX");		  
	  rulesEngine.addRule("F", "C0F/F-[C1-F+F]+[C2+F-F]", 1);
	  rulesEngine.addRule("X", "C0F*F++[C1+F/F]+[C2-FF]", 0.30f);
	  rulesEngine.addRule("X", "C0F**F", 0.70f);
	  //rulesEngine.addRule("]", "L]", 1); //add intermediate leaves
	  
	  ////rulesEngine.addRule("FF", "F*", 0.5f);

	  //basic single branch probability example
	  /*rulesEngine.setAxiom("F");		  
	  rulesEngine.addRule("F", "F+F+F", 0.6f);
	  rulesEngine.addRule("F", "F-F-F", 0.4f);*/

	  std::string result1 = rulesEngine.iterate();
	  std::string result2 = rulesEngine.iterate();
	  std::string result3 = rulesEngine.iterate();
	  rulesEngine.clearRules();
	  rulesEngine.addRule("]", "L]", 1); //add end leaves
	  std::string result4 = rulesEngine.iterate();
	  treeDescription = result4;

	  createTree();  

    }	

	void createTree()
	{
		//Create initial tree
		trees = new std::vector<Tree>(500);

		mat4t root;
		root.loadIdentity();

		tree.init(root, app_scene, trees, meshes, treeDescription, &idGen, framesPerStep);
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

		if (is_key_down(key_ctrl))
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
	  if (is_key_down(key_backspace))
	  {
		  //camera_node->translate(vec3(0, 0.15f, 0));
		  app_scene->reset();
		  trees->clear();
		  app_scene->create_default_camera_and_lights();

		  createTree();
	  }
	  if (is_key_down(key_alt))
	  {
		  camera_node->translate(vec3(0, -0.15f, 0));
	  }


	  //move the camera to fit the tree size
	  the_camera = app_scene->get_camera_instance(0);	  

	  aabb bb = app_scene->get_world_aabb();
	  bb = bb.get_union(aabb(vec3(0, 0, 0), vec3(5, 5, 5)));	 
	  float bb_size = length(bb.get_half_extent()) * 2.0f;
	  float distance = app_scene->max(bb.get_max().z(), bb_size) * 2;
	  vec3 targetPos = vec3(0, bb.get_center().y()+3, distance-27);
	  camera_node->loadIdentity();	 

	  cameraPos = lerp(cameraPos, targetPos, 0.033333f*5);
	  camera_node->translate(cameraPos);
	 
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
