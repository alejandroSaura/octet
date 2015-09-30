////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// invaderer example: simple game with sprites and sounds
//
// Level: 1
//
// Demonstrates:
//   Basic framework app
//   Shaders
//   Basic Matrices
//   Simple game mechanics
//   Texture loaded from GIF file
//   Audio
//

namespace octet {
  class sprite {
    // where is our sprite (overkill for a 2D game!)
    mat4t modelToWorld;

    // half the width of the sprite
    float halfWidth;

    // half the height of the sprite
    float halfHeight;

    // what texture is on our sprite
    int texture;

    // true if this sprite is enabled.
    bool enabled;
  public:
    sprite() {
      texture = 0;
      enabled = true;
    }

    void init(int _texture, float x, float y, float w, float h) {
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      halfWidth = w * 0.5f;
      halfHeight = h * 0.5f;
      texture = _texture;
      enabled = true;
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
         halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };

      // attribute_pos (=0) is position of each corner
      // each corner has 3 floats (x, y, z)
      // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);
    
      // this is an array of the positions of the corners of the texture in 2D
      static const float uvs[] = {
         0,  0,
         1,  0,
         1,  1,
         0,  1,
      };

      // attribute_uv is position in the texture of each corner
      // each corner (vertex) has 2 floats (x, y)
      // there is no gap between the 2 floats and hence the stride is 2*sizeof(float)
      glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)uvs );
      glEnableVertexAttribArray(attribute_uv);
    
      // finally, draw the sprite (4 vertices)
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

	void getPosition( float *X, float *Y){
		(*X) = modelToWorld.w()[0];
		(*Y) = modelToWorld.w()[1];
	}

    // move the object
    void translate(float x, float y) {
      modelToWorld.translate(x, y, 0);
    }

    // position the object relative to another.
    void set_relative(sprite &rhs, float x, float y) {
      modelToWorld = rhs.modelToWorld;
      modelToWorld.translate(x, y, 0);
    }

    // return true if this sprite collides with another.
    // note the "const"s which say we do not modify either sprite
    bool collides_with(const sprite &rhs) const {
      float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];
      float dy = rhs.modelToWorld[3][1] - modelToWorld[3][1];

      // both distances have to be under the sum of the halfwidths
      // for a collision
      return
        (fabsf(dx) < halfWidth + rhs.halfWidth) &&
        (fabsf(dy) < halfHeight + rhs.halfHeight)
      ;
    }

    bool is_above(const sprite &rhs, float margin) const {
      float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];

      return
        (fabsf(dx) < halfWidth + margin)
      ;
    }

    bool &is_enabled() {
      return enabled;
    }
  };

  class invadersMod_app : public octet::app {
    // Matrix to transform points in our camera space to the world.
    // This lets us move our camera
    mat4t cameraToWorld;

    // shader to draw a textured triangle
    texture_shader texture_shader_;

    enum {
      num_sound_sources = 8,
      num_rows = 5,
      num_cols = 8,
      num_missiles = 2,
      num_bombs = 2,
      num_borders = 4,
      num_blocks = num_rows * num_cols,

      // sprite definitions
      player_sprite = 0,
	  ball_sprite,
      game_over_sprite,

      first_block_sprite,
      last_block_sprite = first_block_sprite + num_blocks - 1,      

      first_border_sprite,
      last_border_sprite = first_border_sprite + num_borders - 1,

      num_sprites,

    };

   

    // accounting for bad guys
    int live_blocks;
    int num_lives;

    // game state
    bool game_over;
    int score;

	//speed of player
	float player_velocity;

	//speed of ball
	float ball_velocity_x;
	float ball_velocity_y;
    

    // sounds
    ALuint whoosh;
    ALuint bang;
	ALuint boing;
    unsigned cur_source;
    ALuint sources[num_sound_sources];

    // big array of sprites
    sprite sprites[num_sprites];

    // random number generator
    class random randomizer;

    // a texture for our text
    GLuint font_texture;

    // information for our text
    bitmap_font font;

    ALuint get_sound_source() { return sources[cur_source++ % num_sound_sources]; }

    // called when we hit an enemy
    void on_hit_block() {
      ALuint source = get_sound_source();
      alSourcei(source, AL_BUFFER, bang);
      alSourcePlay(source);

      live_blocks--;
      score++;
      if (live_blocks == 4) {
       
      } else if (live_blocks == 0) {
        game_over = true;
        sprites[game_over_sprite].translate(-20, 0);
      }
    }	

    // use the keyboard to move the ship
    void move_ship() {
      
      // left and right arrows
      if (is_key_down(key_left)) {
        sprites[player_sprite].translate(-player_velocity, 0);
        if (sprites[player_sprite].collides_with(sprites[first_border_sprite+2])) {
          sprites[player_sprite].translate(+player_velocity, 0);
        }
      } else if (is_key_down(key_right)) {
        sprites[player_sprite].translate(+player_velocity, 0);
        if (sprites[player_sprite].collides_with(sprites[first_border_sprite+3])) {
          sprites[player_sprite].translate(-player_velocity, 0);
        }
      }
    }

	void move_ball() {
		sprites[ball_sprite].translate(ball_velocity_x/2, ball_velocity_y/2);

		//collision handlers
		if (sprites[ball_sprite].collides_with(sprites[first_border_sprite + 2]) || sprites[ball_sprite].collides_with(sprites[first_border_sprite + 3])) { //side walls
			ball_velocity_x *= -1;
			
		}

		if (sprites[ball_sprite].collides_with(sprites[first_border_sprite + 1])) { //top wall
			ball_velocity_y *= -1;
			
		}

		if (sprites[ball_sprite].collides_with(sprites[first_border_sprite])){ //bottom wall
			if (--num_lives == 0) {
				game_over = true;
				sprites[game_over_sprite].translate(-20, 0);
			}

			sprites[ball_sprite].set_relative(sprites[first_border_sprite], 0, 3);			

			if (ball_velocity_y < 0) ball_velocity_y *= -1;


		}

		if (sprites[ball_sprite].collides_with(sprites[player_sprite])){				
			on_playerHit();
		}

		for (int j = 0; j != num_blocks; ++j) {
			sprite &block = sprites[first_block_sprite + j];
			if (block.is_enabled() && sprites[ball_sprite].collides_with(block)) {
				block.is_enabled() = false;
				block.translate(20, 0);
				block.is_enabled() = false;
				ball_velocity_y *= -1;
				on_hit_block();

				//goto next_missile;

			//next_missile:;
			}
		}
	}



	void on_playerHit(){

		ball_velocity_y *= -1;

		//the player has two hit areas, so we have to bounce the ball depending
		//on which area was hit.

		float ballX, ballY, playerX, playerY;

		sprites[ball_sprite].getPosition(&ballX, &ballY);
		sprites[player_sprite].getPosition(&playerX, &playerY);

		if ((ballX < playerX) && (ball_velocity_x > 0)){
			ball_velocity_x *= -1;
		}
		if ((ballX > playerX) && (ball_velocity_x < 0)){
			ball_velocity_x *= -1;
		}

	}


    void draw_text(texture_shader &shader, float x, float y, float scale, const char *text) {
      mat4t modelToWorld;
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      modelToWorld.scale(scale, scale, 1);
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

      /*mat4t tmp;
      glLoadIdentity();
      glTranslatef(x, y, 0);
      glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);
      glScalef(scale, scale, 1);
      glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);*/

      enum { max_quads = 32 };
      bitmap_font::vertex vertices[max_quads*4];
      uint32_t indices[max_quads*6];
      aabb bb(vec3(0, 0, 0), vec3(256, 256, 0));

      unsigned num_quads = font.build_mesh(bb, vertices, indices, max_quads, text, 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, font_texture);

      shader.render(modelToProjection, 0);

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].x );
      glEnableVertexAttribArray(attribute_pos);
      glVertexAttribPointer(attribute_uv, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].u );
      glEnableVertexAttribArray(attribute_uv);

      glDrawElements(GL_TRIANGLES, num_quads * 6, GL_UNSIGNED_INT, indices);
    }

  public:

    // this is called when we construct the class
    invadersMod_app(int argc, char **argv) : app(argc, argv), font(512, 256, "assets/big.fnt") {
    }

    // this is called once OpenGL is initialized
    void app_init() {
      // set up the shader
      texture_shader_.init();

      // set up the matrices with a camera 5 units from the origin
      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0, 0, 3);

      font_texture = resource_dict::get_texture_handle(GL_RGBA, "assets/big_0.gif");

      GLuint ship = resource_dict::get_texture_handle(GL_RGBA, "assets/invadersMod/player.gif");
      sprites[player_sprite].init(ship, 0, -2.75f, 0.75f, 0.25f);

	  GLuint ball = resource_dict::get_texture_handle(GL_RGBA, "assets/invadersMod/ball.gif");
	  sprites[ball_sprite].init(ball, 0, -1, 0.25f, 0.25f);

      GLuint GameOver = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/GameOver.gif");
      sprites[game_over_sprite].init(GameOver, 20, 0, 3, 1.5f);

      GLuint block = resource_dict::get_texture_handle(GL_RGBA, "assets/invadersMod/block.gif");
      for (int j = 0; j != num_rows; ++j) {
        for (int i = 0; i != num_cols; ++i) {
          assert(first_block_sprite + i + j*num_cols <= last_block_sprite);
          sprites[first_block_sprite + i + j*num_cols].init(
            block, ((float)i - num_cols * 0.5f) * 0.6f, 2.50f - ((float)j * 0.5f), 0.5f, 0.25f
          );
        }
      }

      // set the border to white for clarity
      GLuint white = resource_dict::get_texture_handle(GL_RGB, "#ffffff");
      sprites[first_border_sprite+0].init(white, 0, -3, 6, 0.2f);
      sprites[first_border_sprite+1].init(white, 0,  3, 6, 0.2f);
      sprites[first_border_sprite+2].init(white, -3, 0, 0.2f, 6);
      sprites[first_border_sprite+3].init(white, 3,  0, 0.2f, 6);      

      // sounds
	  boing = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invadersMod/boing.wav");
	  bang = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/bang.wav");

      cur_source = 0;
      alGenSources(num_sound_sources, sources);

      // sundry counters and game state.
	  player_velocity = 0.1f;
	  ball_velocity_x = 0.1f;
	  ball_velocity_y = 0.1f;
      
      live_blocks = num_blocks;
      num_lives = 3;
      game_over = false;
      score = 0;
    }

    // called every frame to move things
    void simulate() {
      if (game_over) {
        return;
      }

      move_ship();
	  move_ball();

      

      sprite &border = sprites[first_border_sprite+(3)];
      
    }

    // this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      simulate();

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
      for (int i = 0; i != num_sprites; ++i) {
        sprites[i].render(texture_shader_, cameraToWorld);
      }

      char score_text[32];
      sprintf(score_text, "score: %d   lives: %d\n", score, num_lives);
      draw_text(texture_shader_, -1.75f, 2, 1.0f/256, score_text);

      // move the listener with the camera
      vec4 &cpos = cameraToWorld.w();
      alListener3f(AL_POSITION, cpos.x(), cpos.y(), cpos.z());
    }
  };
}
