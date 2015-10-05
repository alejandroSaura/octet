namespace octet{


	class layer {
		string name;

		
		int heigth;

		int lastRow = 0;	
		int lastColumn = 0;

	public:

		int width;
		dynarray<int*> tiles;

		void init(string nam, int h, int w){
			name = nam;
			heigth = h;
			width = w;

			//initialize the array
			for (int i = 0; i < heigth; ++i){
				tiles.push_back(new int[width]);
			}				
		}

		void pushTile(int id){
			
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