# Octet XML scene importer

This project aims to load complete scenes into the Octet framework designed with external tools.
You can find my exporter for Unity3D (http://unity3d.com/) in this project repository: https://github.com/alejandroSaura/LevelDesignerXML.

This loader offers solutions to some compatibility problems between softwares:

- solves the position and rotation differences due to alternative coordinate systems (right hand/left hand).
- adapts this location process using local space rotations instead of octet world space predefined.
- mimics physics behavior written and tested in arbitrary physics engine, making them work with the bullet engine (integrated in octet).

This way, developers can visually locate objects designing a basic scene and add physical behavior between objects without dealing with c++ details.

In the project's actual state only some shapes and features are supported:

- Cubes, paralepipeds
- Spheres
- Rigidbodies (Kinematic and non-kinematic distinction)
- Hinge Joints
- Spring Joints (transformed into bullet's "6 DOF spring constraint [http://bulletphysics.org/Bullet/BulletFull/classbtGeneric6DofSpringConstraint.html])

Video demo: https://www.youtube.com/watch?v=ks-I-f_m1AM

Follow my developments on my blog: https://alejandrosaura.wordpress.com/

## Bug report:
Please be careful with importing geometry without rigidbody, it will not be rotated correctly. As a possible workaround, attach a rigidbody and mark it as kinematic.

