using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using mywowNet;
using System.ComponentModel;

namespace WowModelExplorer.Props
{
    public class WmoProperty : INotifyPropertyChanged
    {
        #region display name

        public const string cateMainModel = "主模型";

        public const string propWMOFile = "WMO名";
        public const string propWMOLongFile = "WMO文件名";

        #endregion

        private string wmoFileName;
        private string wmoLongFileName;

        public void FromWMOSceneNode(WMOSceneNode node)
        {
            SFileWMO filewmo = node.FileWMO;
            WMOFileName = filewmo.name;
            WMOLongFileName = filewmo.longname;
        }

        #region Properties

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propWMOFile)]
        public string WMOFileName
        {
            get { return wmoFileName; }
            set
            {
                if (wmoFileName == value) return;
                wmoFileName = value;
                OnPropertyChanged("WMOFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propWMOLongFile)]
        public string WMOLongFileName
        {
            get { return wmoLongFileName; }
            set
            {
                if (wmoLongFileName == value) return;
                wmoLongFileName = value;
                OnPropertyChanged("WMOLongFileName");
            }
        }

        #endregion

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion
    }

    public class WmoPropertyEnglish : INotifyPropertyChanged
    {
        #region display name

        public const string cateMainModel = "Main WMO";

        public const string propWMOFile = "WMO Name";
        public const string propWMOLongFile = "WMO FileName";


        #endregion

        private string wmoFileName;
        private string wmoLongFileName;

        public void FromWMOSceneNode(WMOSceneNode node)
        {
            SFileWMO filewmo = node.FileWMO;
            WMOFileName = filewmo.name;
            WMOLongFileName = filewmo.longname;
        }

        #region Properties

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propWMOFile)]
        public string WMOFileName
        {
            get { return wmoFileName; }
            set
            {
                if (wmoFileName == value) return;
                wmoFileName = value;
                OnPropertyChanged("WMOFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propWMOLongFile)]
        public string WMOLongFileName
        {
            get { return wmoLongFileName; }
            set
            {
                if (wmoLongFileName == value) return;
                wmoLongFileName = value;
                OnPropertyChanged("WMOLongFileName");
            }
        }

        #endregion

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion
    }
}
