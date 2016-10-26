using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct vector3df
    {
        public vector3df(float x, float y, float z)
        {
            X = x; Y = y; Z = z;
        }
        /// float
        public float X;

        /// float
        public float Y;

        /// float
        public float Z;

        public override bool Equals(object o)
        {
            if (o is vector3df)
                return this == (vector3df)o;
            return base.Equals(o);
        }

        public override int GetHashCode()
        {
            return ToString().GetHashCode();
        }

        public override string ToString()
        {
            return "X = " + X + "; Y = " + Y + "; Z = " + Z + "\"";
        }

        public static vector3df operator +(vector3df first, vector3df other)
        {
            return new vector3df(first.X + other.X, first.Y + other.Y, first.Z + other.Z);
        }

        public static vector3df operator -(vector3df first, vector3df other)
        {
            return new vector3df(first.X - other.X, first.Y - other.Y, first.Z - other.Z);
        }

        public static vector3df operator -(vector3df first)
        {
            return new vector3df(-first.X, -first.Y, -first.Z);
        }

        public static vector3df operator *(vector3df first, float other)
        {
            return new vector3df(first.X * other, first.Y * other, first.Z * other);
        }

        public static vector3df operator /(vector3df first, float other)
        {
            return new vector3df(first.X / other, first.Y / other, first.Z / other);
        }

        public static vector3df operator *(vector3df first, vector3df other)
        {
            return new vector3df(first.X * other.X, first.Y * other.Y, first.Z * other.Z);
        }

        public static vector3df operator /(vector3df first, vector3df other)
        {
            return new vector3df(first.X / other.X, first.Y / other.Y, first.Z / other.Z);
        }

        public static vector3df operator *(float other, vector3df first)
        {
            return new vector3df(first.X * other, first.Y * other, first.Z * other);
        }

        public static vector3df operator /(float other, vector3df first)
        {
            return new vector3df(first.X / other, first.Y / other, first.Z / other);
        }

        public static bool operator ==(vector3df first, vector3df other)
        {
            return NewMath.Equals(first.X, other.X) &&
                NewMath.Equals(first.Y, other.Y) &&
                NewMath.Equals(first.Z, other.Z);
        }

        public static bool operator !=(vector3df first, vector3df other)
        {
            return !(first == other);
        }
   
    }
}
