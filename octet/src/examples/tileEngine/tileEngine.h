////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

using std::string;

namespace octet {



	/// Scene containing a box with octet.
	class tileEngine : public app {

		// Matrix to transform points in our camera space to the world.
		// This lets us move our camera
		mat4t cameraToWorld;

		// shader
		texture_shader texture_shader_;

		int num_sprites = 5000;


		//tilesets		
		std::vector<tileset> tilesets;
		//layers
		std::vector<layer> layers;


	public:

		// library of sprites
		sprite sprites[5000];

		// instanced sprites
		dynarray<sprite> activeSprites;

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

			loadLevel("../../../assets/tileEngine/TestLevel/Test.tmx");

			//draw all layers
			for (std::vector<layer>::size_type i = 0; i != layers.size(); i++) {
				drawLayer(&layers[i]);				
			}
		}

		//load and instantiate all sprites of a given layer
		void drawLayer(layer* lay)
		{
			for (int i = 0; i < lay->tiles.size(); i++)
			{
				for (int j = 0; j < (lay->width) - 1; j++)
				{
					//first we load the sprite in the sprite library
					loadSprite(lay->tiles[i][j]);
					//now we copy the sprite from the library to the active sprites array
					sprites[lay->tiles[i][j]].instantiate(j*0.2f, -i*0.2f, &activeSprites);
				}
			}			
		}

		//Extracts the information from the TMX, creating as many tileset classes
		//and layers as necessary.
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
						loadTileset(pChild);
					}

					if (type == layerType){
						printf("layer found \n", pChild->Value());
						loadLayer(pChild);
					}
				}
			}
		}

		//Load the TMX Dom tree thanks to the TinyXML lib
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

		//fills a tileset class with all the tileset info of the TMX
		void loadLayer(TiXmlNode* pChild)
		{
			layer* lay = new layer();
			string name;
			int width = 0;
			int height = 0;

			TiXmlElement* element = pChild->ToElement();
			TiXmlAttribute* pAttrib = element->FirstAttribute();

			while (pAttrib)
			{
				//printf("%s: value=[%s]", pAttrib->Name(), pAttrib->Value());							
				//printf("\n");
				string attName(pAttrib->Name());

				if (attName.operator==("name")){
					name = pAttrib->Value();
				}
				else if (attName.operator==("width")){
					width = std::stoi(pAttrib->Value());
				}
				else if (attName.operator==("height")){
					height = std::stoi(pAttrib->Value());
				}

				pAttrib = pAttrib->Next();
			}

			lay->init(name, height, width);

			TiXmlNode* pParent = pChild->FirstChild(); //data level (see TMX)

			for (pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
			{
				element = pChild->ToElement();
				pAttrib = element->FirstAttribute();

				while (pAttrib)
				{
					//printf("%s: value=[%s]", pAttrib->Name(), pAttrib->Value());							
					//printf("\n");
					string attName(pAttrib->Name());

					if (attName.operator==("gid")){
						lay->pushTile(std::stoi(pAttrib->Value()));
					}

					pAttrib = pAttrib->Next();
				}
			}

			layers.push_back(*lay);
			//drawLayer(lay);


		}

		//fills a tileset class with all the tileset info of the TMX
		void loadTileset(TiXmlNode* pChild)
		{
			tileset* set = new tileset();

			int firstgid;
			string name;
			string imageSource;
			int width = 0;
			int height = 0;
			int tileWidth = 0;
			int tileHeight = 0;
			int tileCount = 0;

			TiXmlElement* element = pChild->ToElement();
			TiXmlAttribute* pAttrib = element->FirstAttribute();
			//printf("first attribute: %s \n", pAttrib->Value());

			while (pAttrib)
			{
				//printf("%s: value=[%s]", pAttrib->Name(), pAttrib->Value());							
				//printf("\n");
				string attName(pAttrib->Name());


				if (attName.operator==("firstgid")){
					firstgid = std::stoi(pAttrib->Value());
				}
				if (attName.operator==("name")){
					name = pAttrib->Value();
				}
				else if (attName.operator==("tilewidth")){
					tileWidth = std::stoi(pAttrib->Value());
				}
				else if (attName.operator==("tileheight")){
					tileHeight = std::stoi(pAttrib->Value());
				}
				else if (attName.operator==("tilecount")){
					tileCount = std::stoi(pAttrib->Value());
				}

				pAttrib = pAttrib->Next();
			}

			TiXmlElement* imageElement = pChild->FirstChildElement();
			pAttrib = imageElement->FirstAttribute();
			//printf("first attribute: %s \n", pAttrib->Value());

			while (pAttrib)
			{
				//printf("%s: value=[%s]", pAttrib->Name(), pAttrib->Value());							
				//printf("\n");
				string attName(pAttrib->Name());

				if (attName.operator==("source")){
					imageSource = pAttrib->Value();
				}
				else if (attName.operator==("width")){
					width = std::stoi(pAttrib->Value());
				}
				else if (attName.operator==("height")){
					height = std::stoi(pAttrib->Value());
				}

				pAttrib = pAttrib->Next();
			}

			set->init(firstgid, name, imageSource, width, height, tileWidth, tileHeight, tileCount, sprites);

			tilesets.push_back(*set);
		}

		//load the sprite in library, accessing its tileset.
		void loadSprite(int id)
		{
			for (std::vector<tileset>::size_type i = 0; i != tilesets.size(); i++) {
				if (tilesets[i].loadSprite(id))
				{
					return;
				}
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
			for (int i = 0; i != activeSprites.size(); ++i) {
				if (activeSprites[i].is_enabled())
					activeSprites[i].render(texture_shader_, cameraToWorld);
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
