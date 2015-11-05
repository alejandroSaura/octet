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

		int numFrames = 0;

		// Matrix to transform points in our camera space to the world.
		// This lets us move our camera
		mat4t cameraToWorld;

		// shader
		texture_shader texture_shader_;

		int num_sprites = 5000;
		int mapWidth;
		int mapHeight;
		int tileWidth;
		int tileHeight;


		character player;

		//how many frames between pose and pose
		int animationRate = 5;
		const float speed = 0.035f;

		//tilesets		
		std::vector<tileset> tilesets;
		//layers
		std::vector<layer> layers;


	public:

		// library of sprites, we can then instantiate them to use
		sprite sprites[5000];		

		// instanced sprites
		//sprite **activeSprites;
		dynarray<sprite> activeSprites;
		//bool map for collision detection
		bool **collisionMap;

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

			//first sprite (top left) of the character sheet
			float uvs[8] = {
				0, 0.875,
				0.076f, 0.875f,
				0.076f, 1,
				0, 1
			};

			player.loadCharacter("assets/tileEngine/Characters/TerraSheet.gif", 0.4f, -0.4f, 0.2f, 0.2f, uvs, 0.082f, 0.125f, true);

		}

		// called every frame to move things
		void simulate() {
			numFrames++;

			movePlayer();
			if (numFrames%animationRate == 0)
			{
				player.animateCharacter();
			}
		}

		void movePlayer()
		{		
			vec4 playerPos = player.modelToWorld.w();
			
			float playerX = (playerPos[0]+0.1)  / 0.2f;
			float playerY = (playerPos[1]-0.1) / 0.2f;
			if (playerY < 0) playerY *= -1;			

			// left and right arrows
			if (is_key_down(key_left)) {
				player.goLeft();
				cameraToWorld.translate(-speed, 0, 0);
				player.translate(-speed, 0);

				/*printf("player position: %f, %f, %f\n", playerPos[0], playerPos[1], playerPos[2]);
				printf("player position tile: %f, %f\n", playerX, playerY);*/

				int i = (int)(playerX - 0.5f);
				int j = (int)(playerY);
				
				if (collisionMap[j][i] == true)
				{
					cameraToWorld.translate(speed, 0, 0);
					player.translate(speed, 0);
				}

			}
			else if (is_key_down(key_right)) {
				player.goRight();
				cameraToWorld.translate(speed, 0, 0);
				player.translate(speed, 0);

				printf("player position: %f, %f, %f\n", playerPos[0], playerPos[1], playerPos[2]);
				printf("player position tile: %f, %f\n", playerX, playerY);


				int i = (int)(playerX + 0.5f);
				int j = (int)(playerY);

				if (collisionMap[j][i] == true)
				{
					cameraToWorld.translate(-speed, 0, 0);
					player.translate(-speed, 0);
				}
			}
			else if (is_key_down(key_up)) {
				player.goUp();
				cameraToWorld.translate(0, speed, 0);
				player.translate(0, speed);

				int i = (int)(playerX);
				int j = (int)(playerY - 0.5f);

				if (collisionMap[j][i] == true)
				{
					cameraToWorld.translate(0, -speed, 0);
					player.translate(0, -speed);
				}
			}
			else if (is_key_down(key_down)) {
				player.goDown();
				cameraToWorld.translate(0, -speed, 0);
				player.translate(0, -speed);

				int i = (int)(playerX);
				int j = (int)(playerY + 0.5f);

				if (collisionMap[j][i] == true)
				{
					cameraToWorld.translate(0, speed, 0);
					player.translate(0, speed);
				}
			}
			else
			{
				player.goIdle();
			}
		}

		//load and instantiate all sprites of a given layer
		void drawLayer(layer* lay)
		{
			for (int i = 0; i < lay->tiles.size(); i++)
			{
				for (int j = 0; j < (lay->width) - 1; j++)
				{
					if (lay->tiles[i][j] != 0) //we dont want to load or instantiate the void sprite
					{
					
						//first we load the sprite in the sprite library
						loadSprite(lay->tiles[i][j]);

						//intantiate it and store the instance in activeSprites array
						sprite s = sprites[lay->tiles[i][j]].instantiate(j*0.2f, -i*0.2f, lay->getDepth());						
						//activeSprites[j][i] = s;
						activeSprites.push_back(s);
					}
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

			TiXmlElement* element = pParent->ToElement();
			TiXmlAttribute* pAttrib = element->FirstAttribute();

			while (pAttrib)
			{
				//printf("%s: value=[%s]", pAttrib->Name(), pAttrib->Value());							
				//printf("\n");
				string attName(pAttrib->Name());
				if (attName.operator==("width")){
					mapWidth = std::stoi(pAttrib->Value());
				}
				if (attName.operator==("height")){
					mapHeight = std::stoi(pAttrib->Value());
				}
				if (attName.operator==("tilewidth")){
					tileWidth = std::stoi(pAttrib->Value());
				}
				if (attName.operator==("tileheight")){
					tileHeight = std::stoi(pAttrib->Value());
				}
				pAttrib = pAttrib->Next();
			}

			//create the collision map knowing the dimensions of the map
			collisionMap = new bool*[mapHeight];
			for (int i = 0; i < mapWidth; i++)
			{
				collisionMap[i] = new bool[mapWidth];
			}

			//look for tilesets and layers definition
			for (pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
			{
				printf("\n");
				if (pChild->Type() == TiXmlNode::TINYXML_ELEMENT){
					
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

			//Now that we now the dimensions of the map and how many layers we need,
			//lets create the sprites array:
			/*activeSprites = new sprite*[mapHeight];
			for (int i = 0; i < mapWidth; i++)
			{
				activeSprites[i] = new sprite[mapWidth];
			}*/
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

		//fills a layer class with all the layers info of the TMX
		void loadLayer(TiXmlNode* pChild)
		{
			layer lay;
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

				if (attName.operator==("name"))
				{
					name = pAttrib->Value();
				}
				else if (attName.operator==("width"))
				{
					width = std::stoi(pAttrib->Value());
				}
				else if (attName.operator==("height"))
				{
					height = std::stoi(pAttrib->Value());
				}

				pAttrib = pAttrib->Next();
			}

			if (name == "collision") //this will be a special layer, that fills the collision array
			{
				TiXmlNode* pParent = pChild->FirstChild();

				int counter = 0;

				int lastColumn = 0;
				int lastRow = 0;

				for (pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
				{
					element = pChild->ToElement();
					pAttrib = element->FirstAttribute();
						
					while (pAttrib)
					{
						string attName(pAttrib->Name());					

						if (attName.operator==("gid"))
						{			
							if (lastColumn >= width)
							{
								lastRow++;
								lastColumn = 0;
							}
							if (std::stoi(pAttrib->Value()) != 0)
							{
								collisionMap[lastRow][lastColumn] = true;
							}
							else
							{
								collisionMap[lastRow][lastColumn] = false;
							}
							lastColumn += 1;							
						}

						pAttrib = pAttrib->Next();							
					}
				}
			}
			else //not the collision layer
			{
				lay.init(name, height, width);

				//Look for layer's custom parameters
				TiXmlNode* pParentParams = pChild->FirstChild();
				for (TiXmlNode* auxChild = pParentParams->FirstChild(); auxChild != 0; auxChild = auxChild->NextSibling())
				{
					element = auxChild->ToElement();
					pAttrib = element->FirstAttribute();

					while (pAttrib)
					{
						string attName(pAttrib->Name());
						string attValue(pAttrib->Value());
						if (attName.operator==("name") && attValue.operator==("Depth"))
						{
							float depth = 0;
							pAttrib = pAttrib->Next();
							depth = std::stof(pAttrib->Value());
							lay.setDepth(depth);
							break;
						}
						pAttrib = pAttrib->Next();
					}
				}

				TiXmlNode* pParent = pChild->FirstChild()->NextSibling(); //data level (see TMX)

				//Fill the layer tiles info
				for (pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
				{
					element = pChild->ToElement();
					pAttrib = element->FirstAttribute();

					while (pAttrib)
					{						
						string attName(pAttrib->Name());

						if (attName.operator==("gid"))
						{
							lay.pushTileId(std::stoi(pAttrib->Value()));
						}

						pAttrib = pAttrib->Next();
					}
				}
				layers.push_back(lay);
			}
		}

		//fills a tileset class with all the tileset info of the TMX
		void loadTileset(TiXmlNode* pChild)
		{
			tileset set;

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

			while (pAttrib)
			{
				string attName(pAttrib->Name());

				if (attName.operator==("firstgid"))
				{
					firstgid = std::stoi(pAttrib->Value());
				}
				if (attName.operator==("name"))
				{
					name = pAttrib->Value();
				}
				else if (attName.operator==("tilewidth"))
				{
					tileWidth = std::stoi(pAttrib->Value());
				}
				else if (attName.operator==("tileheight"))
				{
					tileHeight = std::stoi(pAttrib->Value());
				}
				else if (attName.operator==("tilecount"))
				{
					tileCount = std::stoi(pAttrib->Value());
				}

				pAttrib = pAttrib->Next();
			}

			TiXmlElement* imageElement = pChild->FirstChildElement();
			pAttrib = imageElement->FirstAttribute();

			while (pAttrib)
			{
				string attName(pAttrib->Name());

				if (attName.operator==("source"))
				{
					imageSource = pAttrib->Value();
				}
				else if (attName.operator==("width"))
				{
					width = std::stoi(pAttrib->Value());
				}
				else if (attName.operator==("height"))
				{
					height = std::stoi(pAttrib->Value());
				}

				pAttrib = pAttrib->Next();
			}

			set.init(firstgid, name, imageSource, width, height, tileWidth, tileHeight, tileCount, sprites);

			tilesets.push_back(set);
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

			simulate();
			// set a viewport - includes whole window area
			glViewport(x, y, w, h);

			// clear the background to black
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// don't allow Z buffer depth testing (closer objects are always drawn in front of far ones)
			glEnable(GL_DEPTH_TEST);

			// allow alpha blend (transparency when alpha channel is 0)
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// draw all the sprites
			/*for (int i = 0; i != mapHeight; ++i) {
				for (int j = 0; j != mapWidth; ++j) {
					if (activeSprites[i][j].is_enabled()){
						activeSprites[i][j].render(texture_shader_, cameraToWorld);
					}
				}
			}	*/		

			
			for (int i = 0; i != activeSprites.size(); ++i) 
			{				
				activeSprites[i].render(texture_shader_, cameraToWorld);				
			}
			player.render(texture_shader_, cameraToWorld);

			

			char score_text[32];
			//sprintf(score_text, "score: %d   lives: %d\n", score, num_lives);
			//draw_text(texture_shader_, -1.75f, 2, 1.0f / 256, score_text);

			// move the listener with the camera
			vec4 &cpos = cameraToWorld.w();
			alListener3f(AL_POSITION, cpos.x(), cpos.y(), cpos.z());
		}

	};
}
