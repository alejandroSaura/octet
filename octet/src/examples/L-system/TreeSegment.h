
namespace octet {

	class TreeSegment
	{
		

	public:		

		mesh_instance *meshI;

		float thicknessSpeed = 0.0005f;

		vec4 initialColor;
		vec4 finalColor;
		int framesUntilFinalColor;
		int t;

		int framesPerStep = 0;
		int currentFrame = 0;
		
		float rotX, rotY, rotZ;
		float thickness;
		material *mate;
		vec4 color;
		float lenght = 1;

		bool visited;

		mat4t transformMatrix;

		octet::TreeNode *startNode;
		octet::TreeNode *endNode;

		mesh_cylinder *cylinder;

		vec4 localUp;
		vec4 localRight;
		vec4 localForward;		

		bool done; // used to know when the segment has reached its max length

		// used to know when the branch where the segment is placed is dead,
		// so the segment stops getting thicker.
		bool dead; 

		void Grow()
		{
			if (dead)
			{
				return;
			}

			thickness += thicknessSpeed;

			if (!done)
			{
				currentFrame++;
			}

			float aux = ((float)t / (float)framesUntilFinalColor);
			color = finalColor*aux + initialColor*(1 - aux);
			//color = vec4(1, 0, 0, 0);
			mate->set_diffuse(color);
			meshI->set_material(mate);
			t++;

			float currentLenght = currentFrame*(lenght / framesPerStep);

			printf("segment growing!\n");
			transformMatrix = startNode->transform;

			mat4t transformMatrixInv;
			transformMatrix.invertQuick(transformMatrixInv);

			mat4t cylTransform;
			cylTransform.loadIdentity();
			cylTransform.rotate(90, 1, 0, 0);
			cylTransform.invertQuick(transformMatrixInv);
			localRight = vec4(0, 1, 0, 0) * transformMatrixInv;
			cylTransform.translate(localRight*currentLenght / 2);

			zcylinder *mathCylinder = new zcylinder(vec3(0, 0, 0), thickness, currentLenght / 2);
			//mesh_cylinder c;
			cylinder->clear_attributes();
			cylinder->set_size(*mathCylinder, cylTransform, 32);

			if (currentFrame > framesPerStep)
			{
				done = true;
			}
			
		}


		TreeNode Init(ref<visual_scene> scene, std::vector<mesh_cylinder> *meshes, int fps)
		{
			framesUntilFinalColor = 100;
			initialColor = vec4(0.6f, 1, 0.2f, 0);
			finalColor = vec4(0.6f, 0.2862f, 0, 0);
			t = 0;
			color = initialColor;


			framesPerStep = fps;
			currentFrame = 0;
			done = false;
			dead = false;
			//locate the start point
			transformMatrix = startNode->transform;

			mat4t transformMatrixInv;
			transformMatrix.invertQuick(transformMatrixInv);
			localForward = vec4(0, 0, 1, 0) * transformMatrixInv;
			transformMatrix.rotate(rotZ, localForward.x(), localForward.y(), localForward.z());

			//rotate around Y axis
			transformMatrix.rotate(rotY, 0, 1, 0);

			//create, rotate and locate the cylinder
			
			{
				mat4t cylTransform;
				cylTransform.loadIdentity();
				cylTransform.rotate(90, 1, 0, 0);
				cylTransform.invertQuick(transformMatrixInv);
				localRight = vec4(0, 1, 0, 0) * transformMatrixInv;
				cylTransform.translate(localRight*lenght / 2);

				zcylinder *mathCylinder = new zcylinder(vec3(0, 0, 0), thickness, lenght / 2);
				cylinder = new mesh_cylinder(*mathCylinder, cylTransform, 32);
				//cylinder.get_mesh()
				//meshes->push_back(cylinder);
			}

			
			//mesh_cylinder m = (*meshes)[meshes->size() - 1];
			//scene->add_shape(transformMatrix, (*meshes)[meshes->size()-1].get_mesh(), new material(color), false);
			mate = new material(color);
			meshI = scene->add_shape(transformMatrix, cylinder, mate, false);

			//scene->add_shape(transformMatrix, new mesh_box(vec3(0.2f, 0.2f, 0.2f)), new material(color), false);	

				

			//printf("segment drawed: rotZ = %2.6f en pos: x=%2.6f, y=%2.6f, z=%2.6f", (rotZ, transformMatrix.colw()[0], transformMatrix.colw()[1], transformMatrix.colw()[2]));

			//create endNode
			TreeNode node;
			node.parent = this;

			//locate endNode		
			node.transform = startNode->transform;
			transformMatrix.invertQuick(transformMatrixInv);			
			localUp = vec4(0, 1, 0, 0) * lenght * transformMatrixInv;
			//node.transform = startNode->transform;
			
			node.transform.translate(localUp.x(), localUp.y(), localUp.z());

			/*node.transform.invertQuick(transformMatrixInv);
			localForward = vec4(0, 0, 1, 0) * transformMatrixInv;
			node.transform.rotate(rotZ, localForward.x(), localForward.y(), localForward.z());*/

			//node.transform.translate(localUp.x(), localUp.y(), localUp.z());

			//array->push_back(node);

			//scene->add_debug_line(startNode->transform.getPosition(), node.transform.getPosition());

			//just for debug purposes
			node.transform.invertQuick(transformMatrixInv);
			localUp = vec4(0, 1, 0, 0) * transformMatrixInv;
			localForward = vec4(0, 0, 1, 0) * transformMatrixInv;
			localRight = vec4(1, 0, 0, 0) * transformMatrixInv;

			vec3 position;
			position.x() = node.transform.w().x();
			position.y() = node.transform.w().y();
			position.z() = node.transform.w().z();

			/*scene->add_debug_line(position, position + localForward);
			scene->add_debug_line(position, position + localRight);
			scene->add_debug_line(position, position + localUp);*/

			//endNode = &node;
			return node;
		}
	};
}