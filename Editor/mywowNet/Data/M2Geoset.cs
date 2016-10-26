using System;
using System.ComponentModel;
using System.Collections.ObjectModel;
using mywowNet;

namespace mywowNet.Data
{
    public class M2Geoset : INotifyPropertyChanged
    {
        public M2Geoset(ref SGeoset geo, int index, bool show)
        {
            Index = index;

            VCount = geo.vcount;
            ICount = geo.icount;
            GeoSetID = geo.geoID;
            TexID = geo.texID;
            RenderFlagsIndex = geo.rfIndex;
            ColorIndex = geo.colorIndex;
            TransparentIndex = geo.transIndex;
            TexAnimationIndex = geo.texAnimIndex;

            Visible = show;
        }

        public int Index { get; set; }

        public uint VCount { get; set; }

        public uint ICount { get; set; }

        public uint GeoSetID { get; set; }

        public short TexID { get; set; }

        public ushort RenderFlagsIndex { get; set; }

        public short ColorIndex { get; set; }

        public short TransparentIndex { get; set; }

        public short TexAnimationIndex { get; set; }

        bool _visible;
        [Browsable(false)]
        public bool Visible
        {
            get { return _visible; }
            set
            {
                bool old = _visible;
                _visible = value;
                if (old != value)
                    NotifyPropertyChanged("Visible");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    public class M2GeosetCollection : ObservableCollection<M2Geoset>
    {

    }
}
