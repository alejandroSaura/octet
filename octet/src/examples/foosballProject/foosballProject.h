////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class foosballProject : public app {
    // scene for drawing box
	  ref<visual_scene> scene;
	sceneLoader* loader = new sceneLoader();
	camera_instance* cam;
	scene_node* camNode;

  public:
    /// this is called when we construct the class before everything is initialised.
    foosballProject(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      scene =  new visual_scene();
      //app_scene->create_default_camera_and_lights();

	  loader->loadScene(scene, "LevelData.xml");
	  cam = scene->get_camera_instance(0);
	  camNode = cam->get_node();
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      scene->update(1.0f/30);

      // draw the scene
      scene->render((float)vx / vy);

	  if (is_key_down(key_left)) 
	  {
		  camNode->translate(vec3(-0.5f, 0, 0));
		  //camNode->rotate(5, vec3(0, 0, 1));
	  }
	  if (is_key_down(key_right))
	  {
		  camNode->translate(vec3(0.5f, 0, 0));
	  }
	  if (is_key_down(key_up))
	  {
		  camNode->translate(vec3(0, 0.5f, 0));
	  }
	  if (is_key_down(key_down))
	  {
		  camNode->translate(vec3(0, -0.5f, 0));
	  }
	  if (is_key_down(key_backspace))
	  {
		  camNode->translate(vec3(0, 0, 0.5f));
	  }
	  if (is_key_down(key_alt))
	  {
		  camNode->translate(vec3(0, 0, -0.5f));
	  }

	  vec3 position = camNode->get_position();
	  
	  //printf("cam position: %f, %f, %f \n", position.x(), position.y(), position.z());
	  
      
    }
  };
}
