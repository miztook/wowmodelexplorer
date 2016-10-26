using System;
using System.ComponentModel;
using System.Collections.ObjectModel;
using mywowNet;

namespace mywowNet.Data
{
        public class WmoGroup : INotifyPropertyChanged
        {
            public WmoGroup(ref SWMOGroup group, int index, bool show, bool boxshow)
            {
                Index = index;

                Name = group.name;

                VCount = group.VCount;
                ICount = group.ICount;
                NumBatches = group.numBatches;
                NumLights = group.numLights;
                NumDoodads = group.numDoodads;
                InDoor = group.Indoor;

                Visible = show;
                BoxVisible = boxshow;
            }

            public string Name { get; set; }

            public int Index { get; set; }

            public uint VCount { get; set; }

            public uint ICount { get; set; }

            public uint NumBatches { get; set; }

            public uint NumLights { get; set; }

            public uint NumDoodads { get; set; }

            public bool InDoor { get; set; }

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

            bool _boxvisible;
            [Browsable(false)]
            public bool BoxVisible
            {
                get { return _boxvisible; }
                set
                {
                    bool old = _boxvisible;
                    _boxvisible = value;
                    if (old != value)
                        NotifyPropertyChanged("BoxVisible");
                }
            }

            public event PropertyChangedEventHandler PropertyChanged;

            void NotifyPropertyChanged(string propertyName)
            {
                if (PropertyChanged != null)
                    PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public class WmoGroupCollection : ObservableCollection<WmoGroup>
        {

        }
}
