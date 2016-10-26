using System;
using System.ComponentModel;
using System.Collections.ObjectModel;
using mywowNet;

namespace mywowNet.Data
{
    public class WmoPortal : INotifyPropertyChanged
    {
        public WmoPortal(ref SWMOPortal portal, int index, bool show, bool showGroups)
        {
            Index = index;

            FrontGroupIndex = portal.frontGroupIndex;
            BackGroupIndex = portal.backGroupIndex;

            Visible = show;
            GroupsVisible = showGroups;
        }

        public int Index { get; set; }

        public uint FrontGroupIndex { get; set; }

        public uint BackGroupIndex { get; set; }

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

        bool _groupsVisible;
        [Browsable(false)]
        public bool GroupsVisible
        {
            get { return _groupsVisible; }
            set
            {
                bool old = _groupsVisible;
                _groupsVisible = value;
                if (old != value)
                    NotifyPropertyChanged("GroupsVisible");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    public class WmoPortalCollection : ObservableCollection<WmoPortal>
    {

    }
}
