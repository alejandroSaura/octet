////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {

	//10 is a temporal value
	const int num_sprites = 10;

	/// Scene containing a box with octet.
	class tileEngine : public app {

		// Matrix to transform points in our camera space to the world.
		// This lets us move our camera
		mat4t cameraToWorld;

		// shader
		texture_shader texture_shader_;

		// big array of sprites
		sprite sprites[num_sprites];

		//tilesets
		std::vector<tileset> tilesets;


	public:
		/// this is called when we construct the class before everything is initialised.
		tileEngine(int argc, char **argv) : app(argc, argv) {
		}		

		/// this is called once OpenGL is initialized
		void app_init() {

			// set up the shader
			texture_shader_.init();

			// set up the matrices with a camera 5 units from the origin
			cameraToWorld.loadIdentity();
			cameraToWorld.translate(0, 0, 3);

			loadLevel("../../../assets/tileEngine/TestLevel/Dungeon.tmx");

			testRender();			
		}

		void testRender(){
			//sprite rendering test:
			GLuint tileset1 = resource_dict::get_texture_handle(GL_RGBA, "assets/tileEngine/Objects/Wall.gif");

			float uvs[] = { //counter-clockwise; 0,0 is the bottom left.
				0, 0,
				0.05f, 0,
				0.05f, 2 * 0.01960784f,
				0, 2 * 0.01960784f
			};

			/*float uvs[] = {
			0, 0,
			1, 0,
			1, 1,
			0, 1,
			};*/

			sprites[0].init(tileset1, 0, 0, 2, 4, uvs);
		}

		void loadLevel(const char* pFilename){

			TiXmlDocument levelTMX;
			levelTMX = loadTMX(pFilename);

			TiXmlNode* pParent = levelTMX.RootElement();
			TiXmlNode* pChild;

			for (pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
			{
				printf("\n");
				if (pChild->Type() == TiXmlNode::TINYXML_ELEMENT){
					//printf("element found! %s \n", pChild->Value());

					string tilesetType = "tileset";
					string layerType = "layer";

					string type(pChild->Value());

					if (type == tilesetType){
						printf("tileset found \n", pChild->Value());

						TiXmlElement* element = pChild->ToElement();
						TiXmlAttribute* pAttrib = element->FirstAttribute();
						printf("first attribute: %s \n", pAttrib->Value());

						tileset* set = new tileset();
						//set->init();
						tilesets.push_back(*set);
					}

					if (type == layerType){
						printf("layer found \n", pChild->Value());
					}

				}
			}
		}

		TiXmlDocument loadTMX(const char* pFilename){

			TiXmlDocument doc(pFilename);
			bool tmxLoaded = doc.LoadFile();

			if (tmxLoaded)
			{
				printf("\nfile loaded: %s\n", pFilename);
				//dump_to_stdout(&doc);
				return doc;
			}
			else
			{
				printf("Failed to load TMX file \"%s\"\n", pFilename);
				return nullptr;
			}

		}

		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {

			// set a viewport - includes whole window area
			glViewport(x, y, w, h);

			// clear the background to black
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// don't allow Z buffer depth testing (closer objects are always drawn in front of far ones)
			glDisable(GL_DEPTH_TEST);

			// allow alpha blend (transparency when alpha channel is 0)
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// draw all the sprites
			for (int i = 0; i != num_sprites; ++i) {
				sprites[i].render(texture_shader_, cameraToWorld);
			}

			char score_text[32];
			//sprintf(score_text, "score: %d   lives: %d\n", score, num_lives);
			//draw_text(texture_shader_, -1.75f, 2, 1.0f / 256, score_text);

			// move the listener with the camera
			vec4 &cpos = cameraToWorld.w();
			alListener3f(AL_POSITION, cpos.x(), cpos.y(), cpos.z());
		}

	};
}
