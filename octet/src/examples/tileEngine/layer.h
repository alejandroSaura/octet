namespace octet{

	class layer {

		string name;	
		int heigth;

		int lastRow = 0;	
		int lastColumn = 0;

		float depth = 0;

	public:

		int width;
		dynarray<int*> tiles; //contains the ids of the textures on each position

		void init(string nam, int h, int w){
			name = nam;
			heigth = h;
			width = w;

			//initialize the array
			for (int i = 0; i < heigth; ++i)
			{
				tiles.push_back(new int[width]);
			}				
		}

		void setDepth(float d)
		{
			depth = d;
		}
		float getDepth()
		{
			return depth;
		}

		void pushTileId(int id){
			
			if (lastColumn >= width)
			{
				lastRow++;
				lastColumn = 0;
			}
			tiles[lastRow][lastColumn] = id;
			lastColumn += 1;
		}

		
	};

}