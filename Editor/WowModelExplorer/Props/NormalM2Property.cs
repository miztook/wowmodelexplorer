using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using mywowNet;
using System.ComponentModel;

namespace WowModelExplorer.Props
{
    public class NormalM2Property : INotifyPropertyChanged
    {
        #region display name
        public const string cateMainModel = "主模型";

        public const string propM2File = "M2名";
        public const string propM2LongFile = "M2文件名";
        public const string propM2Textures = "M2纹理";

        #endregion

        private string m2FileName;
        private string m2LongFileName;
        private string[] m2TextureFileNames;

        public void FromM2SceneNode(M2SceneNode node)
        {
            SFileM2 filem2 = node.FileM2;
            M2FileName = filem2.name;
            M2LongFileName = filem2.longname;
            M2TextureNames = node.TextureNames;
        }

        #region MainModel Properties

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string M2FileName
        {
            get { return m2FileName; }
            set
            {
                if (m2FileName == value) return;
                m2FileName = value;
                OnPropertyChanged("M2FileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string M2LongFileName
        {
            get { return m2LongFileName; }
            set
            {
                if (m2LongFileName == value) return;
                m2LongFileName = value;
                OnPropertyChanged("M2LongFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2Textures)]
        public string[] M2TextureNames
        {
            get { return m2TextureFileNames; }
            set
            {
                if (m2TextureFileNames == value) return;
                m2TextureFileNames = value;
                OnPropertyChanged("M2TextureFileNames");
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

    public class NormalM2PropertyEnglish : INotifyPropertyChanged
    {
        #region display name
        public const string cateMainModel = "Main Model";

        public const string propM2File = "M2 Name";
        public const string propM2LongFile = "M2 FileName";
        public const string propM2Textures = "M2 Textures";

        #endregion

        private string m2FileName;
        private string m2LongFileName;
        private string[] m2TextureFileNames;

        public void FromM2SceneNode(M2SceneNode node)
        {
            SFileM2 filem2 = node.FileM2;
            M2FileName = filem2.name;
            M2LongFileName = filem2.longname;
            M2TextureNames = node.TextureNames;
        }

        #region MainModel Properties

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string M2FileName
        {
            get { return m2FileName; }
            set
            {
                if (m2FileName == value) return;
                m2FileName = value;
                OnPropertyChanged("M2FileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string M2LongFileName
        {
            get { return m2LongFileName; }
            set
            {
                if (m2LongFileName == value) return;
                m2LongFileName = value;
                OnPropertyChanged("M2LongFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2Textures)]
        public string[] M2TextureNames
        {
            get { return m2TextureFileNames; }
            set
            {
                if (m2TextureFileNames == value) return;
                m2TextureFileNames = value;
                OnPropertyChanged("M2TextureFileNames");
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
