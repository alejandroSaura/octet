////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
	/// Scene containing a box with octet.
	class newExampleProject : public app {
		// scene for drawing box
		ref<visual_scene> app_scene;
	public:

		btDiscreteDynamicsWorld* dynamicsWorld;

		/// this is called when we construct the class before everything is initialised.
		newExampleProject(int argc, char **argv) : app(argc, argv) {
		}

		/// this is called once OpenGL is initialized
		void app_init() {
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();



			///-----bullet physics initialization_start-----

			///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
			btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

			///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
			btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);

			///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
			btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

			///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
			btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

			dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

			dynamicsWorld->setGravity(btVector3(0, -10, 0));

			///-----bullet physics initialization_end-----

			///create a few basic test rigid bodies
			btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

			//keep track of the shapes, we release memory at exit.
			//make sure to re-use collision shapes among rigid bodies whenever possible!
			btAlignedObjectArray<btCollisionShape*> collisionShapes;

			collisionShapes.push_back(groundShape);

			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(0, -56, 0));

			{
				btScalar mass(0.);

				//rigidbody is dynamic if and only if mass is non zero, otherwise static
				bool isDynamic = (mass != 0.f);

				btVector3 localInertia(0, 0, 0);
				if (isDynamic)
					groundShape->calculateLocalInertia(mass, localInertia);

				//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
				btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
				btRigidBody* body = new btRigidBody(rbInfo);

				//add the body to the dynamics world
				dynamicsWorld->addRigidBody(body);
			}



			//create  dynamic rigidbody
			{

				//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
				btCollisionShape* colShape = new btSphereShape(btScalar(1.));
				collisionShapes.push_back(colShape);

				/// Create Dynamic Objects
				btTransform startTransform;
				startTransform.setIdentity();

				btScalar	mass(1.f);

				//rigidbody is dynamic if and only if mass is non zero, otherwise static
				bool isDynamic = (mass != 0.f);

				btVector3 localInertia(0, 0, 0);
				if (isDynamic)
					colShape->calculateLocalInertia(mass, localInertia);

				startTransform.setOrigin(btVector3(2, 10, 0));

				//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
				btRigidBody* body = new btRigidBody(rbInfo);

				dynamicsWorld->addRigidBody(body);
			}



		}

		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);


			dynamicsWorld->stepSimulation(1.f / 30.f, 10);

			//print positions of all test objects
			for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
			{
				btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
				btRigidBody* body = btRigidBody::upcast(obj);
				btTransform trans;
				if (body && body->getMotionState())
				{
					body->getMotionState()->getWorldTransform(trans);

				}
				else
				{
					trans = obj->getWorldTransform();
				}
				printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
			}





		}
	};
}
