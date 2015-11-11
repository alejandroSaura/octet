namespace octet
{

	class TreeLeaf
	{
		mesh_instance *meshI;
		mesh *m;
		mat4t transformMatrix;
		material *mat;

		TreeNode *startNode;

	public:

		float leafThickness = 0.05f;
		float leafScale = 1;

		void Init(visual_scene *scene, TreeNode *_startNode)
		{
			startNode = _startNode;

			//locate the start point
			transformMatrix = startNode->transform;

			//create, rotate and locate the box
			mat = new material(vec4(1, 0, 0, 1));
			m = new mesh_box(vec3(0.1, 0.1, 0.1));
			
			scene->add_shape(transformMatrix, m, mat, false);
		}




	};

}