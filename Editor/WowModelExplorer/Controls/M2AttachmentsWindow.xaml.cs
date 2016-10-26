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
using System.Windows.Shapes;
using mywowNet;
using mywowNet.Data;
using System.ComponentModel;
using WowModelExplorer.Data;
using WowModelExplorer.Services;

namespace WowModelExplorer.Controls
{
    /// <summary>
    /// Interaction logic for M2AttachmentsWindow.xaml
    /// </summary>
    public partial class M2AttachmentsWindow : Window
    {
        private ICollectionView attachmentsView;

        public M2AttachmentsWindow()
        {
            InitializeComponent();

            this.Loaded += new RoutedEventHandler(AttachmentsWindow_Loaded);
            this.Closing += (sender, e) => { e.Cancel = true; Hide(); };
        }

        void AttachmentsWindow_Loaded(object sender, RoutedEventArgs e)
        {
            this.Loaded -= AttachmentsWindow_Loaded;

            ModelSceneService.Instance.MainM2SceneNodeChanged += new MainM2SceneNode_ChangedHandler(Instance_MainM2SceneNodeChanged);
        }

        public M2AttachmentCollection Attachments
        {
            get { return _mainGrid.Resources["_attachments"] as M2AttachmentCollection; }
        }

        public void Update()
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            Attachments.Clear();

            if (node == null)
                return;

            int num = (int)node.FileM2.numAttachments;
            for (int i = 0; i < num; ++i)
            {
                M2Attachment a = new M2Attachment(i, node.IsAttachmentShow(i));
                Attachments.Add(a);

                a.PropertyChanged += new PropertyChangedEventHandler(attachment_PropertyChanged);
            }

            attachmentsView = CollectionViewSource.GetDefaultView(Attachments);
        }

        private void Instance_MainM2SceneNodeChanged(object sender, M2SceneNode node)
        {
            Update();
        }

        private void attachment_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            M2Attachment g = sender as M2Attachment;
            if (e.PropertyName == "Visible")
            {
                M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
                if (node != null)
                    node.ShowAttachment(g.Index, g.Visible);
            }
        }

        private void VisibleAllSelect_Click(object sender, RoutedEventArgs e)
        {
            foreach (var g in Attachments)
            {
                g.Visible = true;
            }
        }

        private void VisibleNoneSelect_Click(object sender, RoutedEventArgs e)
        {
            foreach (var g in Attachments)
            {
                g.Visible = false;
            }
        }

        bool indexAscending = true;
        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (attachmentsView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            attachmentsView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Index":
                    {
                        attachmentsView.SortDescriptions.Add(new SortDescription("Index",
                                                indexAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        indexAscending = !indexAscending;
                    }
                    break;
            }
        }
    }
}
