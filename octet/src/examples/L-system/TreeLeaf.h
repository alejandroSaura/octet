# define RADTODEG           57.2957795131f  
# define PI					3.14159265359f

namespace octet

{

	class TreeLeaf
	{
		vec4 color = vec4(0, 0.439f, 0, 1);

		mesh_instance *meshI;
		mesh *m;
		//mat4t transformMatrix;
		material *mat;

		TreeNode *startNode;

		vec3 localUp;
		vec3 localRight;
		vec3 localForward;

	public:

		float leafThickness = 0.01f;
		float leafScale = 2;

		void Init(visual_scene *scene, TreeNode *_startNode)
		{
			startNode = _startNode;			

			////locate the start point
			mat4t transformMatrix = startNode->transform;		
			mat4t transformMatrixInv;

			float rotY = 0; //calculate rotation for the leaf to face opposite to the origin.			
			
			vec3 pos = transformMatrix.getPosition();

			vec2 forw2 = vec2(0, 1);
			vec2 pos2 = vec2(pos.x(), pos.z()).normalize();
			float result = forw2.dot(pos2);
			rotY = std::acosf(result)* RADTODEG;
			if (pos.x() > 0) rotY = 360 - rotY;					

			float s = -sin((rotY)* PI / 180.0f);
			float c = cos((rotY)* PI / 180.0f);
			vec4 direction = vec4(pos2.x(), 0, pos2.y(), 0);
			transformMatrix.translate(direction.xyz() * 0.1f * leafScale);
			

			transformMatrix.invertQuick(transformMatrixInv);
			localUp = (vec4(0, 1, 0, 0) * transformMatrixInv);
			transformMatrix.rotate(rotY, localUp.x(), localUp.y(), localUp.z());

			transformMatrix.invertQuick(transformMatrixInv);
			localRight = (vec4(1, 0, 0, 0) * transformMatrixInv);
			transformMatrix.rotate(-10, localRight.x(), localRight.y(), localRight.z());


			//create and locate the box
			mat = new material(color);
			m = new mesh_box(vec3(0.05f*leafScale, leafThickness*leafScale, 0.1f *leafScale));
			
			scene_node *n = (scene->add_shape(transformMatrix, m, mat, false))->get_node();

			/*transformMatrix.invertQuick(transformMatrixInv);
			localUp = vec4(0, 1, 0, 0) * transformMatrixInv;
			localForward = vec4(0, 0, 1, 0) * transformMatrixInv;
			localRight = vec4(1, 0, 0, 0) * transformMatrixInv;

			vec3 position;
			position.x() = transformMatrix.w().x();
			position.y() = transformMatrix.w().y();
			position.z() = transformMatrix.w().z();

			scene->add_debug_line(position, position + localForward);
			scene->add_debug_line(position, position + localRight);
			scene->add_debug_line(position, position + localUp);*/
		}




	};

}