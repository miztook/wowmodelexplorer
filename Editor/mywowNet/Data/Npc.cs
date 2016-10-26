using System;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace mywowNet.Data
{
    public class Npc : INotifyPropertyChanged
    {
        private string name;
        private string type;
        private int modelDisplayId;
        private int modelId;
        private string path;

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

        public int ModelDisplayId
        {
            get { return modelDisplayId; }
            set
            {
                if (modelDisplayId != value)
                {
                    modelDisplayId = value;
                    RaisePropertyChanged("ModelDisplayId");
                }
            }
        }

        public int ModelId
        {
            get { return modelId; }
            set
            {
                if (modelId != value)
                {
                    modelId = value;
                    RaisePropertyChanged("ModelId");
                }
            }
        }

        public string Path
        {
            get { return path; }
            set
            {
                if (path != value)
                {
                    path = value.ToLower();
                    RaisePropertyChanged("Path");
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

    public class NpcCollection : ObservableCollection<Npc>
    {

    }

}
