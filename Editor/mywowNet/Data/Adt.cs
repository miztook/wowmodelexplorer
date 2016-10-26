using System;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace mywowNet.Data
{
    public class Adt : Entry
    {
        private uint row;
        private uint col;

        public uint Row
        {
            get { return row; }
            set
            {
                if (row != value)
                {
                    row = value;
                    RaisePropertyChanged("Row");
                }
            }
        }

        public uint Col
        {
            get { return col;}
            set
            {
                if (col != value)
                {
                    col = value;
                    RaisePropertyChanged("Col");
                }
            }
        }
    }

    public class AdtCollection : ObservableCollection<Adt>
    {

    }
}
