using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;
using mywowNet.Data;
using WowTexViewer.Services;
using WowTexViewer.Data;

namespace WowTexViewer.Controls
{
    /// <summary>
    /// Interaction logic for TextureTreeViewSelect.xaml
    /// </summary>
    public partial class TextureTreeViewSelect : UserControl
    {
        public delegate void Texture_TreeViewSelectedEventHandler(object sender, TreeNodeEntry texture);

        public TextureTreeViewSelect()
        {
            InitializeComponent();

            _TreeView.AddHandler(TreeViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(treeViewItemDbClicked));
            _TreeView.KeyUp += new KeyEventHandler(treeView_KeyUp);
        }

        public TreeEntryCollection TreeViewFiles
        {
            get { return this._mainGrid.Resources["_textureTreeViewCollection"] as TreeEntryCollection; }
        }

        public void FillData()
        {
            TreeViewFiles.GetTreeItemFiles("", "blp", new TreeNodeEntry());
        }

        public event Texture_TreeViewSelectedEventHandler TextureTreeView_Selected;

        private void treeView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadTexture();
        }

        public void treeViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadTexture();
        }

        private void LoadTexture()
        {
            TreeNodeEntry tex = _TreeView.SelectedItem as TreeNodeEntry;
            if (tex != null)
            {
                if (TextureTreeView_Selected != null)
                    TextureTreeView_Selected.Invoke(this, tex);
            }
        }
    }
}
