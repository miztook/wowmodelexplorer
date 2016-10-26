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
using AvalonDock;
using WowTexViewer.Controls;
using WowTexViewer.Services;
using WowTexViewer.Data;
using mywowNet.Data;

namespace WowTexViewer.Views
{
    /// <summary>
    /// Interaction logic for TextureContent.xaml
    /// </summary>
    public partial class AllTextureContent : DockableContent
    {
        public AllTextureContent()
        {
            InitializeComponent();
            _textureSelect.Texture_Selected += new Texture_SelectedEventHandler(_textureSelect_Texture_Selected);
            _textureTreeViewSelect.TextureTreeView_Selected += new TextureTreeViewSelect.Texture_TreeViewSelectedEventHandler(_textureTreeViewSelect_TextureTreeView_Selected);
        }

        void _textureTreeViewSelect_TextureTreeView_Selected(object sender, TreeNodeEntry texture)
        {
            ModelSceneService.Instance.SetCurrentTextureName(texture.FullName, 1.0f);
        }

        void _textureSelect_Texture_Selected(object sender, Entry tex)
        {
            ModelSceneService.Instance.SetCurrentTextureName(tex.Name, 1.0f);
        }

        public void Initialize()
        {
            _textureSelect.FillData();
            _textureTreeViewSelect.FillData();
        }
    }
}
