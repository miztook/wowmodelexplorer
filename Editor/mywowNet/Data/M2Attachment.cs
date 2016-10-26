using System;
using System.ComponentModel;
using System.Collections.ObjectModel;
using mywowNet;

namespace mywowNet.Data
{
    public class M2Attachment : INotifyPropertyChanged
    {
        public M2Attachment(int index, bool show)
        {
            Index = index;
            Visible = show;
        }

        public int Index { get; set; }

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

    public class M2AttachmentCollection : ObservableCollection<M2Attachment>
    {

    }
}
