using System;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace mywowNet.Data
{
    public class StartOutfitClass : Entry
    {
        private string shortname;

        public string ShortName
        {
            get { return shortname; }
            set
            {
                if (shortname != value)
                {
                    shortname = value;
                    RaisePropertyChanged("ShortName");
                }
            }
        }
    }

    public class StartOutfitClassCollection : ObservableCollection<StartOutfitClass>
    {

    }

}
