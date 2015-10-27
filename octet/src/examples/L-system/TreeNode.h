
namespace octet {

	class TreeSegment;

	class TreeNode
	{
	public:
		mat4t transform;
		octet::TreeSegment *parent;
		dynarray<octet::TreeSegment> children;
	};
}