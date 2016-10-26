using System;
using System.ComponentModel;
using System.Collections.ObjectModel;
using mywowNet;

namespace mywowNet.Data
{
    public class SlotItem : INotifyPropertyChanged
    {
        private string slotName;
        private int itemId;
        private string itemName;
        private string subclassName;

        public string SlotName
        {
            get { return slotName; }
            set
            {
                if (slotName != value)
                {
                    slotName = value;
                    RaisePropertyChanged("SlotName");
                }
            }
        }

        public int ItemId
        {
            get { return itemId; }
            set
            {
                if (itemId != value)
                {
                    itemId = value;
                    RaisePropertyChanged("ItemId");
                }
            }
        }

        public string ItemName
        {
            get { return itemName; }
            set
            {
                if (itemName != value)
                {
                    itemName = value;
                    RaisePropertyChanged("ItemName");
                }
            }
        }

        public string SubClassName
        {
            get { return subclassName; }
            set
            {
                if (subclassName != value)
                {
                    subclassName = value;
                    RaisePropertyChanged("SubClassName");
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

    public class SlotItemCollection : ObservableCollection<SlotItem>
    {

    }
}
