using System;
using System.Collections;
using System.Collections.Generic;

namespace mywowNet
{
    public partial class SceneEnvironment : INative
    {
        public SceneEnvironment(IntPtr raw) : base(raw) { }

        public vector3df LightDir
        {
            set { SceneEnvironment_setLightDir(value); }

            get
            {
                vector3df v = new vector3df();
                SceneEnvrionment_getLightDir(out v);
                return v;
            }
        }

    }
}
