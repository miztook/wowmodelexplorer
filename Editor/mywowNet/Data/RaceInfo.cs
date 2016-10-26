using System;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace mywowNet.Data
{
    public class RaceInfo : INotifyPropertyChanged
    {
        private string name;
        private string shortName;
        private uint raceid;

        public string Name 
        { 
            get { return name;}
            set
            {
                if (name != value)
                {
                    name = value;
                    RaisePropertyChanged("Name");
                }
            }
        }

        public string ShortName
        {
            get { return shortName; }
            set
            {
                if (shortName != value)
                {
                    shortName = value;
                    RaisePropertyChanged("ShortName");
                }
            }
        }

        public uint RaceId
        { 
            get { return raceid; }
            set
            {
                if (raceid != value)
                {
                    raceid = value;
                    RaisePropertyChanged("RaceId");
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

    public class RaceInfoCollection : ObservableCollection<RaceInfo>
    {

    }
}
