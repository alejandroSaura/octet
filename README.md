# Octet Tile Game Technical Demo

This project aims to be a 2D tile-based game technical demo implemented over the octet framework. For that purpose, I have chosen the 2D level editor "Tiled", which allow us to create layers with multiple parameters.

For our demo, I define (in order of increasing depth):
- a ground layer, with depth 0
- the player 
- objects layer
- heights layer (it mixes with the ground layer to represent roofs that occlude the player)
- collision layer (allows us to paint where the player can walk and where not) 

![Alt text](https://github.com/alejandroSaura/octet/blob/tilesEngine/tiledExample.PNG)

This map design is stored in TMX (an XML style format), that holds the definition of: 

- tilesets: large textures with properties:
	- name
	- source file
	- height and width (in tiles and pixels)
	
- layers: description of each layer:
	- which tiles they have and where they must be drawn.	
	- depth 
	
I load this TMX file in Octet with the TinyXML library. Then, I use its implemented functions to explore through the file in the following steps:

1 - Create a big library array that will contain all tiles/sprites definitions (which tileset they use and their uv coordinates).

2 - Iterate through each tileset definition, creating a tileset c++ object containing its properties. This tileset object contains its first and last tile id. That way we can later know if a tile is contained in this tileset.
	
3 - Iterate through each layer definition, creating c++ layer objects. If that layer is the special collisions layer, I use this code to fill a collision array with its values. If its a visual layer, I store in the c++ object the ids of its tiles and their location.
	
4 - I draw all layers, iterating through each layer's tiles and instantiating them using the corresponding tileset. I store the instantiated tiles in a dynamic array.
	
5 - Finally I load a player from another tileset. This player includes a basic animation, changing its uv coordinates with a defined speed (frame rate dependant) and changing uv offsets, selecting different rows of the tileset to represent different movement directions.
	[NOTE: this process is still not generalized, I create a hardcoded character but in next project updates this task should be designed from the Tiled editor]
	
![Alt text](https://github.com/alejandroSaura/octet/blob/tilesEngine/octetGame.PNG)
	
I include in the project a basic shader that receives as input the position of the player and displays only the fragments inside certain distance, giving the feeling of dungeon area.