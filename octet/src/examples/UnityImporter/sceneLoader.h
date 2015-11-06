

namespace octet
{
	class sceneLoader
	{

	public:		

		void loadScene(ref<visual_scene> scene, const char* pFilename)
		{
			TiXmlDocument sceneXML;
			
			sceneXML = loadXML(pFilename);

			if (sceneXML.Error() == true)
			{
				scene->create_default_camera_and_lights();
				return;
			}

			TiXmlHandle docHandle(&sceneXML);
			
			TiXmlElement* geometryRoot = sceneXML.FirstChildElement("ArrayOfGeometry");
			TiXmlElement* lightsRoot = sceneXML.FirstChildElement("ArrayOfOctetLight");
			TiXmlElement* camerasRoot = sceneXML.FirstChildElement("ArrayOfOctetCamera");
			TiXmlElement* rigidBodiesRoot = sceneXML.FirstChildElement("ArrayOfOctetRigidBody");
			TiXmlElement* hingeJointsRoot = sceneXML.FirstChildElement("ArrayOfOctetHingeJoint");
			TiXmlElement* springJointsRoot = sceneXML.FirstChildElement("ArrayOfOctetSpringJoint");

			TiXmlElement* child;
			
			if (geometryRoot != nullptr)
			{
				child = geometryRoot->FirstChildElement();
				for (child; child; child = child->NextSiblingElement())
				{
					printf("Geometry object found\n");

					int id;
					string type;
					float posX, posY, posZ;
					float rotX, rotY, rotZ;
					float scaleX, scaleY, scaleZ;

					TiXmlElement* element;

					type = child->FirstChildElement("geometryType")->GetText();

					element = child->FirstChildElement("nodeId");
					id = atof(element->GetText());


					element = child->FirstChildElement("posX");
					posX = atof(element->GetText());

					element = child->FirstChildElement("posY");
					posY = atof(element->GetText());

					element = child->FirstChildElement("posZ");
					posZ = atof(element->GetText());


					element = child->FirstChildElement("rotX");
					rotX = atof(element->GetText());

					element = child->FirstChildElement("rotY");
					rotY = atof(element->GetText());

					element = child->FirstChildElement("rotZ");
					rotZ = atof(element->GetText());


					element = child->FirstChildElement("scaleX");
					scaleX = atof(element->GetText());

					element = child->FirstChildElement("scaleY");
					scaleY = atof(element->GetText());

					element = child->FirstChildElement("scaleZ");
					scaleZ = atof(element->GetText());


					mat4t transformMatrix;
					transformMatrix.loadIdentity();
					transformMatrix.translate(posX, posY, -posZ);

					vec4 localUp;
					vec4 localForward;
					vec4 localRight;

					mat4t transformMatrixInv;

					transformMatrix.invertQuick(transformMatrixInv);
					localUp = vec4(0, 1, 0, 0) * transformMatrixInv;
					localForward = vec4(0, 0, 1, 0) * transformMatrixInv;
					localRight = vec4(1, 0, 0, 0) * transformMatrixInv;

					transformMatrix.rotate(rotX, localRight.x(), localRight.y(), localRight.z());

					transformMatrix.invertQuick(transformMatrixInv);
					localUp = vec4(0, 1, 0, 0) * transformMatrixInv;
					localForward = vec4(0, 0, 1, 0) * transformMatrixInv;
					localRight = vec4(1, 0, 0, 0) * transformMatrixInv;

					transformMatrix.rotate(rotY, localUp.x(), localUp.y(), localUp.z());

					transformMatrix.invertQuick(transformMatrixInv);
					localUp = vec4(0, 1, 0, 0) * transformMatrixInv;
					localForward = vec4(0, 0, 1, 0) * transformMatrixInv;
					localRight = vec4(1, 0, 0, 0) * transformMatrixInv;

					transformMatrix.rotate(-rotZ, localForward.x(), localForward.y(), localForward.z());

					

					//just for debug purposes
					transformMatrix.invertQuick(transformMatrixInv);
					localUp = vec4(0, 1, 0, 0) * transformMatrixInv;
					localForward = vec4(0, 0, 1, 0) * transformMatrixInv;
					localRight = vec4(1, 0, 0, 0) * transformMatrixInv;

					vec3 position;
					position.x() = transformMatrix.w().x();
					position.y() = transformMatrix.w().y();
					position.z() = transformMatrix.w().z();

					material *debug_material;

					debug_material = new material(vec4(0, 0, 1, 1));
					scene->setDebugMaterial(debug_material);
					scene->add_debug_line(position, position + localForward);

					debug_material = new material(vec4(0, 1, 0, 1));
					scene->setDebugMaterial(debug_material);
					scene->add_debug_line(position, position + localRight);

					debug_material = new material(vec4(1, 0, 0, 1));
					scene->setDebugMaterial(debug_material);
					scene->add_debug_line(position, position + localUp);


					//transformMatrix.rotateX(-rotX);
					//transformMatrix.rotateY(-rotY);
					////transformMatrix.rotateZ(-rotZ);
					//transformMatrix.rotate(-rotZ, 0, 0, 1);

					material *mat = new material(vec4(1, 0.928f, 0.716f, 1)); //TO-DO: import materials

					if (type.operator==("Cube"))
					{
						//scene->add_shape(transformMatrix, new mesh_box(vec3(scaleX / 2, scaleY / 2, scaleZ / 2)), mat, false);
						scene->add_geometry(transformMatrix, new mesh_box(vec3(scaleX / 2, scaleY / 2, scaleZ / 2)), mat, id);							
					}
					else if (type.operator==("Sphere"))
					{
						mesh_sphere *sphere = new mesh_sphere(vec3(posX, posY, -posZ), scaleX/2);
						scene->add_geometry(transformMatrix, sphere, mat, id);
						//scene->add_shape(transformMatrix, sphere, mat, true);
					}

				}
			}


			if (lightsRoot != nullptr)
			{
				child = lightsRoot->FirstChildElement();
				for (child; child; child = child->NextSiblingElement())
				{
					printf("Light object found\n");

					string type;
					float posX, posY, posZ;
					float rotX, rotY, rotZ;
					float colorR, colorG, colorB;

					TiXmlElement* element;

					type = child->FirstChildElement("lightType")->GetText();

					element = child->FirstChildElement("posX");
					posX = atof(element->GetText());

					element = child->FirstChildElement("posY");
					posY = atof(element->GetText());

					element = child->FirstChildElement("posZ");
					posZ = atof(element->GetText());


					element = child->FirstChildElement("rotX");
					rotX = atof(element->GetText());

					element = child->FirstChildElement("rotY");
					rotY = atof(element->GetText());

					element = child->FirstChildElement("rotZ");
					rotZ = atof(element->GetText());


					element = child->FirstChildElement("colorR");
					colorR = atof(element->GetText());

					element = child->FirstChildElement("colorG");
					colorG = atof(element->GetText());

					element = child->FirstChildElement("colorB");
					colorB = atof(element->GetText());


					if (type.operator==("Point"))
					{
						scene_node *node = scene->add_scene_node();
						light *_light = new light();
						light_instance *li = new light_instance();
						node->access_nodeToParent().loadIdentity();
						node->access_nodeToParent().translate(posX, posY, -posZ);
						/*node->access_nodeToParent().rotateX(-rotX);
						node->access_nodeToParent().rotateY(-rotY);
						node->access_nodeToParent().rotateZ(-rotZ);*/
						_light->set_color(vec4(colorR, colorG, colorB, 1));							
						_light->set_kind(atom_point);
						li->set_node(node);
						li->set_light(_light);
						scene->add_light_instance(li);
						
					}


				}
			}

			if (camerasRoot != nullptr)
			{
				child = camerasRoot->FirstChildElement();
				for (child; child; child = child->NextSiblingElement())
				{
					printf("Camera object found\n");

					float posX, posY, posZ;
					float rotX, rotY, rotZ;

					TiXmlElement* element;

					element = child->FirstChildElement("posX");
					posX = atof(element->GetText());

					element = child->FirstChildElement("posY");
					posY = atof(element->GetText());

					element = child->FirstChildElement("posZ");
					posZ = atof(element->GetText());


					element = child->FirstChildElement("rotX");
					rotX = atof(element->GetText());

					element = child->FirstChildElement("rotY");
					rotY = atof(element->GetText());

					element = child->FirstChildElement("rotZ");
					rotZ = atof(element->GetText());					

					aabb bb = scene->get_world_aabb();
					bb = bb.get_union(aabb(vec3(0, 0, 0), vec3(5, 5, 5)));
					scene_node *node = scene->add_scene_node();
					camera_instance *cam = new camera_instance();
					float bb_size = length(bb.get_half_extent()) * 2.0f;
					float distance = scene->max(bb.get_max().z(), bb_size) * 2;
					node->access_nodeToParent().translate(0, 0, 0);
					float f = distance * 2, n = f * 0.001f;
					cam->set_node(node);
					cam->set_perspective(0, 45, 1, n, f);
					scene->add_camera_instance(cam);

					//scene->create_default_camera_and_lights();					

					mat4t &cameraToWorld = node->access_nodeToParent();
					cameraToWorld.translate(vec3(posX, posY, -posZ));

					cameraToWorld.rotateY(-rotY);
					cameraToWorld.rotateX(-rotX);
					cameraToWorld.rotateZ(rotZ);


				}
			}
			else
			{
				scene->create_default_camera_and_lights();
			}

			if (rigidBodiesRoot != nullptr)
			{
				child = rigidBodiesRoot->FirstChildElement();
				for (child; child; child = child->NextSiblingElement())
				{
					printf("RigidBody object found\n");

					int id;
					bool kinematic;

					TiXmlElement* element;

					string b = child->FirstChildElement("Kinematic")->GetText();
					kinematic = b.operator==("true");

					if (!kinematic) printf("non kinematic rigidbody found\n");					

					element = child->FirstChildElement("nodeId");
					id = atof(element->GetText());
										
					scene->add_rigidbody(id, !kinematic);
				}
			}

			if (hingeJointsRoot != nullptr)
			{
				child = hingeJointsRoot->FirstChildElement();
				for (child; child; child = child->NextSiblingElement())
				{
					printf("HingeJoint object found\n");

					int idFrom;
					int idTo;					

					btVector3 axis;
					btVector3 anchor;
					btVector3 anchorB;

					TiXmlElement* element;

					element = child->FirstChildElement("nodeIdFrom");
					idFrom = atof(element->GetText());

					element = child->FirstChildElement("nodeIdTO");
					idTo = atof(element->GetText());


					element = child->FirstChildElement("axisX");
					axis.setX (atof(element->GetText()));
					element = child->FirstChildElement("axisY");
					axis.setY (atof(element->GetText()));
					element = child->FirstChildElement("axisZ");
					axis.setZ (-atof(element->GetText()));

					element = child->FirstChildElement("anchorX");
					anchor.setX (atof(element->GetText()));
					element = child->FirstChildElement("anchorY");
					anchor.setY (atof(element->GetText()));
					element = child->FirstChildElement("anchorZ");
					anchor.setZ (-atof(element->GetText()));

					element = child->FirstChildElement("anchorBX");
					anchorB.setX(atof(element->GetText()));
					element = child->FirstChildElement("anchorBY");
					anchorB.setY(atof(element->GetText()));
					element = child->FirstChildElement("anchorBZ");
					anchorB.setZ(-atof(element->GetText()));

					scene_node *nodeFrom = scene->getNode(idFrom);
					scene_node *nodeTo = scene->getNode(idTo);

					btRigidBody *rbFrom = nodeFrom->get_rigid_body();
					btRigidBody *rbTo = nodeTo->get_rigid_body();

					btHingeConstraint *HingeConstraint = new btHingeConstraint(*rbFrom, *rbTo, anchor, anchorB, axis, axis, true);
					HingeConstraint->setDbgDrawSize(btScalar(5.f));
					scene->addHingeConstraint(HingeConstraint);
					
				}
			}	

			if (springJointsRoot != nullptr)
			{
				child = springJointsRoot->FirstChildElement();
				for (child; child; child = child->NextSiblingElement())
				{
					printf("SpringJoint object found\n");

					int idFrom;
					int idTo;

					float springFactor;
					
					btVector3 anchor;
					btVector3 anchorB;

					TiXmlElement* element;

					element = child->FirstChildElement("nodeIdFrom");
					idFrom = atof(element->GetText());

					element = child->FirstChildElement("nodeIdTO");
					idTo = atof(element->GetText());


					element = child->FirstChildElement("springFactor");
					springFactor = atof(element->GetText());					

					element = child->FirstChildElement("anchorX");
					anchor.setX(atof(element->GetText()));
					element = child->FirstChildElement("anchorY");
					anchor.setY(atof(element->GetText()));
					element = child->FirstChildElement("anchorZ");
					anchor.setZ(-atof(element->GetText()));

					element = child->FirstChildElement("anchorBX");
					anchorB.setX(atof(element->GetText()));
					element = child->FirstChildElement("anchorBY");
					anchorB.setY(atof(element->GetText()));
					element = child->FirstChildElement("anchorBZ");
					anchorB.setZ(-atof(element->GetText()));



					scene_node *nodeFrom = scene->getNode(idFrom);
					scene_node *nodeTo = scene->getNode(idTo);

					btRigidBody *rbFrom = nodeFrom->get_rigid_body();
					btRigidBody *rbTo = nodeTo->get_rigid_body();								
					
					btTransform frameInA = btTransform::getIdentity();
					frameInA.setOrigin(anchor);
					btTransform frameInB = btTransform::getIdentity();
					frameInB.setOrigin(anchorB);

					btGeneric6DofSpringConstraint *springConstraint = new btGeneric6DofSpringConstraint(*rbFrom, *rbTo, frameInA, frameInB, true);					

					springConstraint->setStiffness(0, springFactor);
					springConstraint->setStiffness(1, springFactor);
					springConstraint->setStiffness(2, springFactor);
					springConstraint->setStiffness(3, springFactor);
					springConstraint->setStiffness(4, springFactor);
					springConstraint->setStiffness(5, springFactor);

					scene->addSpringConstraint(springConstraint);

				}
			}


		}

		//Load the TMX Dom tree thanks to the TinyXML lib
		TiXmlDocument loadXML(const char* pFilename){

			TiXmlDocument doc(pFilename);
			bool tmxLoaded = doc.LoadFile();

			if (tmxLoaded)
			{
				printf("\nfile loaded: %s\n", pFilename);
				//dump_to_stdout(&doc);
				return doc;
			}
			else
			{
				printf("Failed to load TMX file \"%s\"\n", pFilename);
				return doc;
			}

		}
	};
}