namespace octet{


	class character {

		float uvWidth;
		float uvHeight;

		// what texture is on our sprite
		int texture;

		//uv coordinates
		float uvCoord[8];

		int step;
		float row = 0;

	public:
		// half the width of the sprite
		float halfWidth;

		// half the height of the sprite
		float halfHeight;

		mat4t modelToWorld;

		void goLeft()
		{
			if (state != LEFT)
			{
				state = LEFT;
				animateCharacter();
			}
		}
		void goRight()
		{
			if (state != RIGHT)
			{
				state = RIGHT;
				animateCharacter();
			}
		}
		void goUp()
		{
			if (state != UP)
			{
				state = UP;
				animateCharacter();
			}
		}
		void goDown()
		{
			if (state != DOWN)
			{
				state = DOWN;
				animateCharacter();
			}
		}
		void goIdle()
		{
			if (state != IDLE)
			{
				state = IDLE;
				//animateCharacter();
			}
		}

		enum STATES
		{
			UP,
			RIGHT,
			DOWN,
			LEFT,
			IDLE
		};

		STATES state;

		bool enabled;

		void loadCharacter(string route, float posX, float posY, float w, float h, float uvs[8], float uvW, float uvH, bool en)
		{
			state = DOWN;

			halfWidth = w * 0.5f;
			halfHeight = h * 0.5f;
			for (int i = 0; i < 8; i++)
			{
				uvCoord[i] = uvs[i];
			}

			uvWidth = uvW;
			uvHeight = uvH;

			texture = texture = resource_dict::get_texture_handle(GL_RGBA, route.c_str());;
			enabled = en;

			modelToWorld.loadIdentity();
			modelToWorld.translate(posX, posY, 0.05f);

			step = 0;

		}

		// move the object
		void translate(float x, float y) {
			modelToWorld.translate(x, y, 0);
		}

		void animateCharacter()
		{


			switch (state)
			{
			case UP:  row = 3;  break;
			case RIGHT: row = 2; break;
			case DOWN: row = 0;  break;
			case LEFT: row = 1; break;
			}

			uvCoord[0] = (uvWidth*step);
			uvCoord[1] = 1 - (row*uvHeight + uvHeight);
			uvCoord[2] = (uvWidth*step + uvWidth);
			uvCoord[3] = 1 - (row*uvHeight + uvHeight);
			uvCoord[4] = (uvWidth*step + uvWidth);
			uvCoord[5] = 1 - (row*uvHeight);
			uvCoord[6] = (uvWidth*step);
			uvCoord[7] = 1 - row*uvHeight;

			if (state != IDLE) step++;
			if (step > 2) step = 0;
		}

		void render(texture_shader &shader, mat4t &cameraToWorld) {
			// invisible sprite... used for gameplay.
			if (!texture) return;

			// build a projection matrix: model -> world -> camera -> projection
			// the projection space is the cube -1 <= x/w, y/w, z/w <= 1
			mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

			// set up opengl to draw textured triangles using sampler 0 (GL_TEXTURE0)
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);

			// use "old skool" rendering
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			shader.render(modelToProjection, 0);

			// this is an array of the positions of the corners of the sprite in 3D
			// a straight "float" here means this array is being generated here at runtime.
			float vertices[] = {
				-halfWidth, -halfHeight, 0,
				halfWidth, -halfHeight, 0,
				halfWidth, halfHeight, 0,
				-halfWidth, halfHeight, 0,
			};

			// attribute_pos (=0) is position of each corner
			// each corner has 3 floats (x, y, z)
			// there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
			glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices);
			glEnableVertexAttribArray(attribute_pos);

			// this is an array of the positions of the corners of the texture in 2D
			/*static const float uvs[] = {
			0, 0,
			1, 0,
			1, 1,
			0, 1,
			};*/

			// attribute_uv is position in the texture of each corner
			// each corner (vertex) has 2 floats (x, y)
			// there is no gap between the 2 floats and hence the stride is 2*sizeof(float)
			glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)uvCoord);
			glEnableVertexAttribArray(attribute_uv);

			// finally, draw the sprite (4 vertices)
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}



	};

}