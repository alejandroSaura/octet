namespace octet {

	class tileset {

		char* name;
		char* imageSource;

		int width;
		int height;

		int tileWidth;
		int tileHeight;

		int horizontalTiles;
		int verticalTiles;

	public:
		void init(char* nam, char* imSo, int w, int h, int tileW, int tileH){

			name = nam;
			imageSource = imSo;
			width = w;
			height = h;
			tileWidth = tileW;
			tileHeight = tileH;

			horizontalTiles = width / tileWidth;
			verticalTiles = height / tileHeight;
		}


	};


}