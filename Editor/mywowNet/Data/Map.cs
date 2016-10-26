using System;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace mywowNet.Data
{
    public class Map : Entry
    {
        private string type;

        public string Type
        {
            get { return type; }
            set
            {
                if (type != value)
                {
                    type = value;
                    RaisePropertyChanged("Type");
                }
            }
        }
    }

    public class MapCollection : ObservableCollection<Map>
    {

    }
}
