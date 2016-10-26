using System;
using System.ComponentModel;
using System.Collections.ObjectModel;
using mywowNet;

namespace mywowNet.Data
{
    public class M2State : INotifyPropertyChanged
    {
        private E_M2_STATES state;
        private string name;

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

        public E_M2_STATES State
        {
            get { return state; }
            set
            {
                if (state != value)
                {
                    state = value;
                    RaisePropertyChanged("State");
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

    public class M2StateCollection : ObservableCollection<M2State>
    {

    }
}
