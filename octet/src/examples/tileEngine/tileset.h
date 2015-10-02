namespace octet {

	class tileset {

		const char* name;
		const char* imageSource;

		int width;
		int height;

		int tileWidth;
		int tileHeight;

		int horizontalTiles;
		int verticalTiles;

	public:
		void init(const char* nam, const char* imSo, int w, int h, int tileW, int tileH){

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