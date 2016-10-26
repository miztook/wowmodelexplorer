using System;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace mywowNet.Data
{
    public class Entry : INotifyPropertyChanged
    {
        private string name;
        private int id;

        public string Name
        {
            get { return name; }
            set
            {
                if (name != value)
                {
                    name = value;
                    RaisePropertyChanged("Name");
                }
            }
        }

        public int Id
        {
            get { return id; }
            set
            {
                if(id != value)
                {
                    id = value;
                    RaisePropertyChanged("Id");
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
        }

    }

    public class EntryCollection : ObservableCollection<Entry>
    {

    }
}
