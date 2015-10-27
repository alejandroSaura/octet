
namespace octet {

	class TreeSegment
	{
	public:		
		
		float rotX, rotY, rotZ;
		float thickness;
		vec4 color;
		float lenght = 1;

		bool visited;

		octet::TreeNode *startNode;
		octet::TreeNode *endNode;


		TreeNode Init(ref<visual_scene> scene)
		{
			mat4t transformMatrix = startNode->transform;
			mat4t transformMatrixInv;
			transformMatrix.invertQuick(transformMatrixInv);
			vec4 localForward = vec4(0, 0, 1, 0) * transformMatrixInv;
			transformMatrix.rotate(rotZ, localForward.x(), localForward.y(), localForward.z());
			scene->add_shape(transformMatrix, new mesh_box(vec3(0.2f, 0.2f, 0.2f)), new material(color), false);

			

			//printf("segment drawed: rotZ = %2.6f en pos: x=%2.6f, y=%2.6f, z=%2.6f", (rotZ, transformMatrix.colw()[0], transformMatrix.colw()[1], transformMatrix.colw()[2]));

			//create endNode
			TreeNode node;
			node.parent = this;

			//locate endNode			
			transformMatrix.invertQuick(transformMatrixInv);			
			vec4 localUp = vec4(0, 1, 0, 0) * lenght * transformMatrixInv;
			node.transform = startNode->transform;
			node.transform.translate(localUp.x(), localUp.y(), localUp.z());
			//array->push_back(node);

			//endNode = &node;
			return node;
		}
	};
}