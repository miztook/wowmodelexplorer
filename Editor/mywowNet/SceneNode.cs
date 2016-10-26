using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class SceneNode : INative
    {
        public SceneNode(IntPtr raw) : base(raw) { }

        public bool Visible
        {
            get { return SceneNode_getVisible(pointer); }
            set { SceneNode_setVisible(pointer, value); }
        }

        public void SetRelativeTransformation(ref matrix4 mat)
        {
            SceneNode_setRelativeTransformation(pointer, ref mat);
        }

        public void GetRelativeTransformation(out matrix4 mat)
        {
            SceneNode_getRelativeTransformation(pointer, out mat);
        }

        public void Update(bool includeChildren)
        {
            SceneNode_update(pointer, includeChildren);
        }
    }
}
