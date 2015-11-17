
namespace octet {

	class TreeSegment
	{
		

	public:		

		mesh_instance *meshI;

		//float thicknessSpeed = 0.0005f;

		float thicknessSpeed = 0.0025f;

		vec4 initialColor = vec4(0.50196078431f, 0.50196078431f, 0, 0);
		vec4 finalColor = vec4(0.271f, 0.192f, 0.047f, 0);
		int framesUntilFinalColor = 10;
		int t;

		int framesPerStep = 2;
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

		// when the branch where the segment is placed is dead,
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
			mate->set_diffuse(color);
			meshI->set_material(mate);
			t++;
			if (t > framesUntilFinalColor) t = framesUntilFinalColor;

			float currentLenght = currentFrame*(lenght / framesPerStep);

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
			cylinder->clear_attributes();
			cylinder->set_size(*mathCylinder, cylTransform, 8);

			if (currentFrame > framesPerStep-1)
			{
				done = true;
			}
			
		}

		TreeNode Init(ref<visual_scene> scene, MeshPool *cylmeshes, int fps, vec4 _initialColor, vec4 _finalColor, int _framesUntilFinalColor)
		{
			initialColor = _initialColor;
			finalColor = _finalColor;
			framesUntilFinalColor = _framesUntilFinalColor;
			
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

				zcylinder *mathCylinder = new zcylinder(vec3(0, 0, 0), thickness, 0);
				cylinder = new mesh_cylinder(*mathCylinder, cylTransform, 32);	

				/*cylinder = cylmeshes->getCylinderMesh();
				cylinder->clear_attributes();
				cylinder->init(*mathCylinder, cylTransform, 8);*/
			}		
			
			mate = new material(color);
			meshI = scene->add_shape(transformMatrix, cylinder, mate, false);

			//create endNode
			TreeNode node;
			node.parent = this;

			//locate endNode		
			node.transform = startNode->transform;
			transformMatrix.invertQuick(transformMatrixInv);			
			localUp = vec4(0, 1, 0, 0) * lenght * transformMatrixInv;
			
			node.transform.translate(localUp.x(), localUp.y(), localUp.z());			

			//just for debug purposes
			/*node.transform.invertQuick(transformMatrixInv);
			localUp = vec4(0, 1, 0, 0) * transformMatrixInv;
			localForward = vec4(0, 0, 1, 0) * transformMatrixInv;
			localRight = vec4(1, 0, 0, 0) * transformMatrixInv;

			vec3 position;
			position.x() = node.transform.w().x();
			position.y() = node.transform.w().y();
			position.z() = node.transform.w().z();*/

			/*scene->add_debug_line(position, position + localForward);
			scene->add_debug_line(position, position + localRight);
			scene->add_debug_line(position, position + localUp);*/

			//endNode = &node;
			return node;
		}
	};
}