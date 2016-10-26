using System;
using System.ComponentModel;

namespace mywowNet.Data
{
    public class CharFeature : INotifyPropertyChanged
    {
        private uint skinColor;
        private uint faceType;
        private uint hairColor;
        private uint hairStyle;
        private uint facialHair;

        public uint SkinColor
        {
            get { return skinColor; }
            set
            {
                if (skinColor != value)
                {
                    skinColor = value;
                    RaisePropertyChanged("SkinColor");
                }
            }
        }

        public uint FaceType
        {
            get { return faceType; }
            set
            {
                if (faceType != value)
                {
                    faceType = value;
                    RaisePropertyChanged("FaceType");
                }
            }
        }

        public uint HairColor
        {
            get { return hairColor; }
            set
            {
                if (hairColor != value)
                {
                    hairColor = value;
                    RaisePropertyChanged("HairColor");
                }
            }
        }

        public uint HairStyle
        {
            get { return hairStyle; }
            set
            {
                if (hairStyle != value)
                {
                    hairStyle = value;
                    RaisePropertyChanged("HairStyle");
                }
            }
        }

        public uint FacialHair
        {
            get { return facialHair; }
            set
            {
                if (facialHair != value)
                {
                    facialHair = value;
                    RaisePropertyChanged("FacialHair");
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
