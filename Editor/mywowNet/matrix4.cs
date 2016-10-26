using System;
using System.Runtime.InteropServices;
using System.Text;

namespace mywowNet
{
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct matrix4
    {
        /// float[16]
        [MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst = 16, ArraySubType = UnmanagedType.R4)]
        public float[] M;

        public matrix4(bool identity)
        {
            M = new float[16];
            if (identity)
                makeIdentity();
        }

        void makeIdentity()
        {
            M[0] = M[5] = M[10] = M[15] = 1;
            M[1] = M[2] = M[3] = M[4] =
            M[6] = M[7] = M[8] = M[9] =
            M[11] = M[12] = M[13] = M[14] = 0;
        }

        int getIndex(int row, int col) { return row*4 + col; }

        public float this[int index]
        {
            get { return M[index]; }
            set { M[index] = value; }
        }

        public float this[int row, int col]
        {
            get { return M[row+4+col]; }
            set { M[row*4+col] = value; }
        }

        public bool isIdentity()
        {
            if (!NewMath.Equals(M[0], 1) ||
        !NewMath.Equals(M[5], 1) ||
        !NewMath.Equals(M[10], 1) ||
        !NewMath.Equals(M[15], 1))
                return false;

            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    if ((j != i) && (!NewMath.IsZero(M[i*4+j])))
                        return false;

            return true;
        }

        public bool isZero()
        {
            for (int i = 0; i < 16; ++i)
            {
                if (!NewMath.Equals(M[i], 0))
                    return false;
            }
            return true;
        }

        public vector3df RotationRadians
        {
            get { return getRotationRadians(); }
            set { setRotationRadians(value); }
        }

        public vector3df RotationDegrees
        {
            get { return getRotationDegrees(); }
            set { setRotationDegrees(value); }
        }

        public vector3df Translation
        {
            get { return getTranslation(); }
            set { setTranslation(value); }
        }

        public vector3df Scale
        {
            get { return getScale(); }
            set { setScale(value);  }
        }

        void setRotationRadians(vector3df rotation)
        {
            float cr = NewMath.FCos(rotation.X);
            float sr = NewMath.FSin(rotation.X);
            float cp = NewMath.FCos(rotation.Y);
            float sp = NewMath.FSin(rotation.Y);
            float cy = NewMath.FCos(rotation.Z);
            float sy = NewMath.FSin(rotation.Z);

            M[0] = (float)(cp * cy);
            M[1] = (float)(cp * sy);
            M[2] = (float)(-sp);

            double srsp = sr * sp;
            double crsp = cr * sp;

            M[4] = (float)(srsp * cy - cr * sy);
            M[5] = (float)(srsp * sy + cr * cy);
            M[6] = (float)(sr * cp);

            M[8] = (float)(crsp * cy + sr * sy);
            M[9] = (float)(crsp * sy - sr * cy);
            M[10] = (float)(cr * cp);
        }

        vector3df getRotationRadians()
        {
            matrix4 mat = this;
	        vector3df scale = getScale();
	        vector3df invScale = new vector3df(1.0f/scale.X, 1.0f/scale.Y, 1.0f/scale.Z);

	        float Y = -NewMath.FASin(mat[2]*invScale.X);
	        float C = NewMath.FCos(Y);

	        float rotx, roty, X, Z;

            if (Math.Abs(C) > 0.0005f)
            {
                float invC = 1.0f/C;
                rotx = mat[10] * invC * invScale.Z;
                roty = mat[6] * invC * invScale.Y;
                X = NewMath.FATan2(roty, rotx);
                rotx = mat[0] * invC * invScale.X;
                roty = mat[1] * invC * invScale.X;
                Z = NewMath.FATan2(roty, rotx);
            }
            else
            {
                X = 0;
                rotx = mat[5] * invScale.Y;
                roty = -mat[4] * invScale.Y;
                Z = NewMath.FATan2(roty, rotx);
            }

	        // fix values that get below zero
	        // before it would set (!) values to 360
	        // that were above 360:
	        if (X < 0.0) X += 2 * NewMath.PI;
            if (Y < 0.0) Y += 2 * NewMath.PI;
            if (Z < 0.0) Z += 2 * NewMath.PI;

            return new vector3df(X, Y, Z);
        }

        void setRotationDegrees(vector3df rotation)
        {
            setRotationRadians(rotation * NewMath.DEGTORAD);
        }

        vector3df getRotationDegrees()
        {
            return getRotationRadians() * NewMath.RADTODEG;
        }

        void setTranslation(vector3df translation)
        {
            M[12] = translation.X;
            M[13] = translation.Y;
            M[14] = translation.Z;
        }

        vector3df getTranslation()
        {
            return new vector3df(M[12], M[13], M[14]);
        }

        void setScale(vector3df scale)
        {
            if (isIdentity())
	        {
		        M[0] = scale.X;
		        M[5] = scale.Y;
		        M[10] = scale.Z;
	        }

            matrix4 mat = new matrix4(true);
	        mat[0] = scale.X;
	        mat[5] = scale.Y;
	        mat[10] = scale.Z;
            
            matrix4 self = this;
            setByProduct( ref self, ref mat);
        }

        vector3df getScale()
        {
            if (NewMath.IsZero(M[1]) && NewMath.IsZero(M[2]) &&
        NewMath.IsZero(M[4]) && NewMath.IsZero(M[6]) &&
        NewMath.IsZero(M[8]) && NewMath.IsZero(M[9]))
                return new vector3df(M[0], M[5], M[10]);

            // We have to do the full calculation.
            return new vector3df(NewMath.Sqrt(M[0] * M[0] + M[1] * M[1] + M[2] * M[2]),
                NewMath.Sqrt(M[4] * M[4] + M[5] * M[5] + M[6] * M[6]),
                NewMath.Sqrt(M[8] * M[8] + M[9] * M[9] + M[10] * M[10]));
        }

        void setByProduct( ref matrix4 other_a, ref matrix4 other_b)
        {
            float[] m1 = other_a.M;
            float[] m2 = other_b.M;

            float[] tmp = new float[16];

            tmp[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];
            tmp[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
            tmp[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
            tmp[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];

            tmp[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
            tmp[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
            tmp[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
            tmp[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];

            tmp[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
            tmp[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
            tmp[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
            tmp[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];

            tmp[12] = m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12];
            tmp[13] = m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13];
            tmp[14] = m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14];
            tmp[15] = m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15];

            for (int i = 0; i < 16; ++i)
                M[i] = tmp[i];
        }

        public static matrix4 operator *(matrix4 first, matrix4 other)
        {
            matrix4 mat = new matrix4(true);
            mat.setByProduct(ref first, ref other);
            return mat;
        }

        public static bool operator!=(matrix4 first, matrix4 other)
        {
            return !(first == other);
        }

        public static bool operator ==(matrix4 first, matrix4 other)
        {
            for (int i = 0; i < 16; ++i)
                if (!NewMath.Equals(first[i], other[i]))
                    return false;
            return true;
        }

        public override bool Equals(object o)
        {
            if (o is matrix4)
                return this == (matrix4)o;
            return base.Equals(o);
        }

        public override int GetHashCode()
        {
            return M.GetHashCode();
        }

        public override string ToString()
        {
            StringBuilder s = new StringBuilder();
            s.AppendFormat("%f,%f,%f,%f, %f,&f,%f,%f, %f,%f,%f,%f, %f,%f,%f,%f", 
                M[0], M[1], M[2], M[3], M[4], M[5], M[6], M[7], M[8], M[9], M[10], M[11], M[12], M[13], M[14], M[15]);
            return s.ToString();
        }

    }
}
