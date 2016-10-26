using System;
using System.IO;
using System.Data;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows.Media.Imaging;
using System.Windows;
using mywowNet;

namespace WowModelExplorer.Data
{
    public static class TreeViewEntry
    {
        private static BitmapSource fileImage;
        private static BitmapSource folderImage;

        static TreeViewEntry()
        {
            fileImage = Application.Current.FindResource("FileImage") as BitmapSource;
            folderImage = Application.Current.FindResource("FolderImage") as BitmapSource;
        }

        public static TreeNodeEntry TraverseAll(string path, string extension, TreeNodeEntry node)
        {
            WowEnvironment env = Engine.Instance.WowEnvironment;

            FileDirIInfo[] allFiles = env.GetFilesInDir(path, extension, false);
            for (int i = 0; i < allFiles.Length; i++)
            {
                TreeNodeEntry fileNode = new TreeNodeEntry();
                fileNode.Name = allFiles[i].Name;
                fileNode.FullName = allFiles[i].FullName;
                fileNode.Icon = fileImage;
                node.NodeEntrys.Add(fileNode);
            }

            FileDirIInfo[] allDir = env.GetDirectoriesInDir(path, false);
            for (int d = 0; d < allDir.Length; d++)
            {
                TreeNodeEntry folderNode = new TreeNodeEntry();
                folderNode.Name = allDir[d].Name;
                folderNode.Icon = folderImage;

                node.NodeEntrys.Add(folderNode);
                TraverseAll(allDir[d].FullName, extension, folderNode);
            }
            return node;
        }

        public static TreeNodeEntry TraverseAllUseOwn(string path, string extension, TreeNodeEntry node)
        {
            WowEnvironment env = Engine.Instance.WowEnvironment;

            FileDirIInfo[] allFiles = env.GetFilesInDir(path, extension, true);
            for (int i = 0; i < allFiles.Length; i++)
            {
                TreeNodeEntry fileNode = new TreeNodeEntry();
                fileNode.Name = allFiles[i].Name;
                fileNode.FullName = allFiles[i].FullName;
                fileNode.Icon = fileImage;
                node.NodeEntrys.Add(fileNode);
            }

            FileDirIInfo[] allDir = env.GetDirectoriesInDir(path, true);
            for (int d = 0; d < allDir.Length; d++)
            {
                TreeNodeEntry folderNode = new TreeNodeEntry();
                folderNode.Name = allDir[d].Name;
                folderNode.Icon = folderImage;

                node.NodeEntrys.Add(folderNode);
                TraverseAllUseOwn(allDir[d].FullName, extension, folderNode);
            }
            return node;
        }

        public static void GetTreeItemFiles(this TreeEntryCollection FilesCollection, string path, string extension, TreeNodeEntry node)
        {
            FilesCollection.Clear();
            TreeNodeEntry treeNode = TraverseAll(path, extension, node);
            for (int i = 0; i < treeNode.NodeEntrys.Count; i++)
            {
                FilesCollection.Add(treeNode.NodeEntrys[i]);
            }
        }
    }

    //treeViewNode
    public class TreeNodeEntry : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public TreeNodeEntry()
        {
            this.nodeEntrys = new ObservableCollection<TreeNodeEntry>();

            id = 0;
            name = "";
            fullname = "";
        }

        private int id;
        public int ID
        {
            get { return id; }
            set { id = value; this.OnPropertyChanged("ID"); }
        }

        private string name;
        public string Name
        {
            get { return name; }
            set { name = value; this.OnPropertyChanged("Name"); }
        }

        private string fullname;
        public string FullName
        {
            get { return fullname; }
            set { fullname = value; this.OnPropertyChanged("FullName"); }
        }

        private BitmapSource icon;
        public BitmapSource Icon
        {
            get { return icon; }
            set { icon = value; this.OnPropertyChanged("Icon"); }
        }

        private ObservableCollection<TreeNodeEntry> nodeEntrys;
        public ObservableCollection<TreeNodeEntry> NodeEntrys
        {
            get { return nodeEntrys; }
            set { nodeEntrys = value; this.OnPropertyChanged("NodeEntrys"); }
        }

        public void OnPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }

    public class TreeEntryCollection : ObservableCollection<TreeNodeEntry>
    {

    }
}
