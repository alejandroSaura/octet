////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Single texture shader with no lighting

namespace octet { namespace shaders {
  class texture_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjectionIndex_;
	GLuint modelToWorldIndex_;
	//index for the player position
	GLuint playerPosIndex_;

    // index for texture sampler
    GLuint samplerIndex_;
  public:
    void init() {
      // this is the vertex shader.
      // it is called for each corner of each triangle
      // it inputs pos and uv from each corner
      // it outputs gl_Position and uv_ to the rasterizer
      const char vertex_shader[] = SHADER_STR(
        varying vec2 uv_;		
		varying vec4 world_pos;
		varying vec4 player_pos;

        attribute vec4 pos;
        attribute vec2 uv;

        uniform mat4 modelToProjection;		
		uniform mat4 modelToWorld;
		uniform vec4 playerPos;

        void main() 
		{ 
			world_pos = modelToWorld * pos;
			gl_Position = modelToProjection * pos; 
			uv_ = uv; 
			player_pos = playerPos;
		}
      );

      // this is the fragment shader
      // after the rasterizer breaks the triangle into fragments
      // this is called for every fragment
      // it outputs gl_FragColor, the color of the pixel and inputs uv_
      const char fragment_shader[] = SHADER_STR(
		 
        varying vec2 uv_;

		varying vec4 world_pos;
		varying vec4 player_pos;

        uniform sampler2D sampler;

		void main()
		{ 
			float d = distance(player_pos.xy, world_pos.xy);
			if (d < 0.8f)
			{
				gl_FragColor = texture2D(sampler, uv_);

			}
			else
			{
				gl_FragColor = vec4(0, 0, 0, 0);
			}
		}
      );
    
      // use the common shader code to compile and link the shaders
      // the result is a shader program
      shader::init(vertex_shader, fragment_shader);

      // extract the indices of the uniforms to use later
      modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
      samplerIndex_ = glGetUniformLocation(program(), "sampler");
	  modelToWorldIndex_ = glGetUniformLocation(program(), "modelToWorld");
	  playerPosIndex_ = glGetUniformLocation(program(), "playerPos");

	  
    }

	void render(const mat4t &modelToProjection, const mat4t &modelToWorld, vec3 playerPos, int sampler) {
      // tell openGL to use the program
      shader::render();

      // customize the program with uniforms
      glUniform1i(samplerIndex_, sampler);
      glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
	  glUniformMatrix4fv(modelToWorldIndex_, 1, GL_FALSE, modelToWorld.get());
	  glUniform4f(playerPosIndex_, playerPos.x(), playerPos.y(), playerPos.z(), 1.0f);

    }
  };
}}
