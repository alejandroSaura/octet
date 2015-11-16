# define RADTODEG           57.2957795131  /* pi */


namespace octet

{

	class TreeLeaf
	{
		vec4 color = vec4(0, 0.439f, 0, 1);

		mesh_instance *meshI;
		mesh *m;
		mat4t transformMatrix;
		material *mat;

		TreeNode *startNode;

	public:

		float leafThickness = 0.01f;
		float leafScale = 1;

		void Init(visual_scene *scene, TreeNode *_startNode)
		{
			startNode = _startNode;

			//create and locate the box
			mat = new material(color);
			m = new mesh_box(vec3(0.1f*leafScale, leafThickness*leafScale, 0.05f *leafScale));

			scene_node *node = (scene->add_shape(transformMatrix, m, mat, false))->get_node();

			//locate the start point
			transformMatrix = startNode->transform;		
			mat4t transformMatrixInv;

			float rotY = 0; //calculate rotation for the leaf to face opposite to the origin.

			transformMatrix.invertQuick(transformMatrixInv);
			vec4 localForward = (vec4(0, 0, 1, 0) * transformMatrixInv);
			vec3 pos = transformMatrix.getPosition();

			/*vec2 forw2 = vec2(localForward.x(), localForward.z()).normalize();
			vec2 pos2 = vec2(pos.x(), pos.z()).normalize();
			float result = forw2.dot(pos2);
			rotY = std::acosf(result)* RADTODEG;*/
			transformMatrix.rotate(45, 0, 1, 0);

			/*transformMatrix.invertQuick(transformMatrixInv);
			vec4 localRight = (vec4(1, 0, 0, 0) * transformMatrixInv);
			transformMatrix.rotate(-35, localRight.x(), localRight.y(), localRight.z());*/

			transformMatrix.invertQuick(transformMatrixInv);
			//vec4 localRigth = (vec4(1, 0, 0, 0) * transformMatrixInv);
			//transformMatrix.translate(localRigth.xyz() *0.1f *leafScale / 2);
			localForward = (vec4(0, 0, 1, 0) * transformMatrixInv);
			//transformMatrix.translate(-localForward.xyz() *0.05f *leafScale / 2);

			transformMatrix.translate(localForward.xyz() * 0.1f);


			//create and locate the box
			mat = new material(color);
			m = new mesh_box(vec3(0.1f*leafScale, leafThickness*leafScale, 0.05f *leafScale));
			
			scene_node *n = (scene->add_shape(transformMatrix, m, mat, false))->get_node();
		}




	};

}