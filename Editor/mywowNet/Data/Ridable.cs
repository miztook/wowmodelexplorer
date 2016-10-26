using System;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace mywowNet.Data
{
    public class Ridable : INotifyPropertyChanged
    {
        private int npcid;
        private uint mountflag;
        private string name;
        private int modelDisplayId;
        private int modelId;

        public int NpcId
        {
            get { return npcid; }
            set
            {
                if (npcid != value)
                {
                    npcid = value;
                    RaisePropertyChanged("NpcId");
                }
            }
        }

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

        public uint MountFlag
        {
            get { return mountflag; }
            set
            {
                if (mountflag != value)
                {
                    mountflag = value;
                    RaisePropertyChanged("MountFlag");
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

        public event PropertyChangedEventHandler PropertyChanged;

        protected void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }

    public class RidableCollection : ObservableCollection<Ridable>
    {

    }
}
