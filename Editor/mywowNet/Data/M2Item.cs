using System;
using System.ComponentModel;
using System.Collections.ObjectModel;
using mywowNet;

namespace mywowNet.Data
{
    public class M2Item : Item
    {
        private string modelPath;
        private string texturePath;

        public string ModelPath
        {
            get { return modelPath; }
            set
            {
                if (modelPath != value)
                {
                    modelPath = value;
                    RaisePropertyChanged("ModelPath");
                }
            }
        }

        public string TexturePath
        {
            get { return texturePath; }
            set
            {
                if (texturePath != value)
                {
                    texturePath = value;
                    RaisePropertyChanged("TexturePath");
                }
            }
        }
    }

    public class M2ItemCollection : ObservableCollection<M2Item>
    {

    }
}
