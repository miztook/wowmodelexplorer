using System;
using System.ComponentModel;
using System.Collections.ObjectModel;
using mywowNet;

namespace mywowNet.Data
{
    public class Item : Entry
    {
        private int modelId;
        private E_ITEM_TYPE type;
        private string subclassName;

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

        public E_ITEM_TYPE Type
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
    }

    public class ItemCollection : ObservableCollection<Item>
    {
        
    }
}
