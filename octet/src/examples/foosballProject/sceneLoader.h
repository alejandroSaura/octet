namespace octet
{
	/// Scene containing a box with octet.
	class sceneLoader
	{

	public:
		
		void loadScene(ref<visual_scene> scene, const char* pFilename)
		{
			TiXmlDocument sceneXML;
			sceneXML = loadXML(pFilename);

			TiXmlHandle docHandle(&sceneXML);
			
			TiXmlElement* geometryRoot = sceneXML.FirstChildElement("ArrayOfGeometry");
			TiXmlElement* lightsRoot = sceneXML.FirstChildElement("ArrayOfOctetLight");
			TiXmlElement* camerasRoot = sceneXML.FirstChildElement("ArrayOfOctetCamera");
			TiXmlElement* rigidBodiesRoot = sceneXML.FirstChildElement("ArrayOfOctetRigidBody");

			TiXmlElement* child;
			
			if (geometryRoot != nullptr)
			{
				child = geometryRoot->FirstChildElement();
				for (child; child; child = child->NextSiblingElement())
				{
					printf("Geometry object found\n");

					string type;
					float posX, posY, posZ;
					float rotX, rotY, rotZ;
					float scaleX, scaleY, scaleZ;

					TiXmlElement* element;

					type = child->FirstChildElement("geometryType")->GetText();

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
					transformMatrix.translate(posX, posY, -posZ);
					transformMatrix.rotateX(-rotX);
					transformMatrix.rotateY(-rotY);
					transformMatrix.rotateZ(-rotZ);

					material *mat = new material(vec4(1, 0, 0, 1)); //TO-DO: import materials

					if (type.operator==("Cube"))
					{
						scene->add_shape(transformMatrix, new mesh_box(vec3(scaleX / 2, scaleY / 2, scaleZ / 2)), mat, false);
					}
					else if (type.operator==("Sphere"))
					{
						mesh_sphere *sphere = new mesh_sphere(vec3(posX, posY, -posZ), scaleX/2);
						scene->add_shape(transformMatrix, sphere, mat, false);
					}

				}
			}


			if (lightsRoot != nullptr)
			{
				child = lightsRoot->FirstChildElement();
				for (child; child; child = child->NextSiblingElement())
				{
					printf("Light object found\n");
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


					/*mat4t transformMatrix;
					transformMatrix.loadIdentity();
					transformMatrix.translate(posX, posY, posZ);
					transformMatrix.rotateX(rotX);
					transformMatrix.rotateY(rotY);
					transformMatrix.rotateZ(rotZ);*/
					
					//camera_instance* cam = scene->get_camera_instance(0);
					//scene_node* node = cam->get_node();

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

					scene->create_default_camera_and_lights();					
					
					node->access_nodeToParent().translate(vec3(posX, posY, -posZ));
					
					/*node->access_nodeToParent().rotateX(-rotX);
					node->access_nodeToParent().rotateY(-rotY);
					node->access_nodeToParent().rotateZ(rotZ);*/

					node->rotate(-rotX, vec3(1, 0, 0));
					node->rotate(-rotY, vec3(0, 1, 0));
					node->rotate(rotZ, vec3(0, 0, 1));

				}
			}

			if (rigidBodiesRoot != nullptr)
			{
				child = rigidBodiesRoot->FirstChildElement();
				for (child; child; child = child->NextSiblingElement())
				{
					printf("RigidBody object found\n");
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
				return nullptr;
			}

		}
	};
}