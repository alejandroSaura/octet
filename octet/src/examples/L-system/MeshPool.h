#define POOL_SIZE 0

namespace octet
{
	class MeshPool
	{
		//mesh_cylinder meshes[POOL_SIZE];
		std::vector<mesh_cylinder> *meshes;

	public:

		MeshPool()
		{
			//meshes->resize(5000)
			//for (int i = 0; i < POOL_SIZE; i++)
			//{
			//	mat4t m;
			//	m.loadIdentity();
			//	zcylinder *mathCylinder = new zcylinder(vec3(0, 0, 0), 1, 1);
			//	mesh_cylinder c;
			//	c.init(*mathCylinder, m, 8);
			//	//meshes->push_back(c);
			//}
			meshes = new std::vector<mesh_cylinder>(POOL_SIZE);
			//meshes->resize(POOL_SIZE);
			/*for (int i = 0; i < POOL_SIZE; i++)
			{
				mat4t m;
				m.loadIdentity();
				zcylinder *mathCylinder = new zcylinder(vec3(0, 0, 0), 1, 1);
				mesh_cylinder c;
				c.init(*mathCylinder, m, 8);
				meshes[i] = c;
			}*/
		}

		mesh_cylinder* getCylinderMesh()
		{
			/*for (int i = 0; i < POOL_SIZE; i++)
			{
				if (!(*meshes)[i].inUse())
				{					
					(*meshes)[i].setUsed(true);
					return &((*meshes)[i]);
				}
			}*/
			return nullptr;
		}

		void resetPool()
		{
			/*for (int i = 0; i < POOL_SIZE; i++)
			{
				(*meshes)[i].setUsed(false);
			}*/
		}
	};
}