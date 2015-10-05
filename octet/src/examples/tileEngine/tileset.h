namespace octet {

#define TILE_WIDTH  0.2
#define TILE_HEIGHT  0.2


	class tileset {

		int firstgid;

		string name;
		std::string imageSource;

		int tileCount;

		float width;
		float height;

		float tileWidth;
		float tileHeight;

		int horizontalTiles;
		int verticalTiles;		

		GLuint texture;

		sprite* sprites;

	public:

		

		void init(int fid, string nam, string imSo, int w, int h, int tileW, int tileH, int tileC, sprite* s){

			firstgid = fid;
			name = nam;
			imageSource = imSo;
			width = (float)w;
			height = (float)h;
			tileWidth = (float)tileW;
			tileHeight = (float)tileH;
			tileCount = tileC;
			sprites = s;
			

			horizontalTiles = width / tileWidth;
			verticalTiles = height / tileHeight;

			std::string route = "assets/tileEngine";
			imageSource.erase(0,2);
			route += imageSource;
			texture = resource_dict::get_texture_handle(GL_RGBA, route.c_str());

			populateSprites();
		}

		void populateSprites()
		{
			int counter = 0;
			for (int i = firstgid; i < firstgid + tileCount; i++)
			{
				float frac = (float)counter / (float)horizontalTiles;
				int whole = (int)frac;
				float f = frac - (int)frac;
				int y = verticalTiles - whole -1;
				int x = ((int)(f * horizontalTiles + 0.5f));

				
				float uvs[] = { //counter-clockwise; 0,0 is the bottom left.
					(x * tileWidth)/width, (y * tileHeight)/height,
					(x * tileWidth + tileWidth) / width, (y * tileHeight) / height,
					(x * tileWidth + tileWidth) / width, (y * tileHeight + tileHeight) / height,
					(x * tileWidth) / width, (y * tileHeight + tileHeight) / height
				};

				sprites[i].init(texture, TILE_WIDTH, TILE_HEIGHT, uvs, false);

				counter++;
			}

		}


	};


}