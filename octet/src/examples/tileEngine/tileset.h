namespace octet {

	class tileset {

		int firstgid;

		string name;
		std::string imageSource;

		int tileCount;

		int width;
		int height;

		int tileWidth;
		int tileHeight;

		int horizontalTiles;
		int verticalTiles;		

		GLuint texture;

		sprite* sprites;

	public:

		

		void init(int fid, string nam, string imSo, int w, int h, int tileW, int tileH, int tileC, sprite* s){

			firstgid = fid;
			name = nam;
			imageSource = imSo;
			width = w;
			height = h;
			tileWidth = tileW;
			tileHeight = tileH;
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
			for (int i = firstgid; i < firstgid + tileCount; i++)
			{
				float uvs[] = { //counter-clockwise; 0,0 is the bottom left.
					0, 0,
					0.05f, 0,
					0.05f, 2 * 0.01960784f,
					0, 2 * 0.01960784f
				};

				sprites[i].init(texture, 0, 0, 2, 4, uvs, false);
			}

		}


	};


}