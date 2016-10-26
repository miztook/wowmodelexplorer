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
using System.IO;
using Microsoft.Win32;

namespace WowModelExplorer.Controls
{
    /// <summary>
    /// Interaction logic for ModelEquipments.xaml
    /// </summary>
    public partial class M2EquipmentsWindow : Window
    {
        public M2EquipmentsWindow()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(ModelEquipments_Loaded);
            this.Closing += (sender, e) => { e.Cancel = true; Hide(); };
        }

        void ModelEquipments_Loaded(object sender, RoutedEventArgs e)
        {
            this.Loaded -= ModelEquipments_Loaded;

            App app = Application.Current as App;
            app.ApplicationLanguageChanged += new Application_LanguageChangedHandler(app_ApplicationLanguageChanged);

            ModelSceneService.Instance.MainM2SceneNodeChanged += new MainM2SceneNode_ChangedHandler(Instance_MainM2SceneNodeChanged);

            _listView.PreviewDragOver += new DragEventHandler(_listView_PreviewDragOver);
            _listView.PreviewDrop += new DragEventHandler(_listView_PreviewDrop);
        }

        void _listView_PreviewDragOver(object sender, DragEventArgs args)
        {
            if (IsSingleFile(args) != "") args.Effects = DragDropEffects.Copy;
            else args.Effects = DragDropEffects.None;

            // Mark the event as handled, so TextBox's native DragOver handler is not called.
            args.Handled = true;
        }
        
        void _listView_PreviewDrop(object sender, DragEventArgs args)
        {
            args.Handled = true;

            string fileName = IsSingleFile(args);
            if (fileName == null) return;

            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node == null || node.Type != M2Type.MT_CHARACTER || node.IsNpc)
                return;

            M2Appearance appearance = node.M2Appearance;
            if (appearance == null)
                return;

            StreamReader reader = new StreamReader(fileName);

            while (!reader.EndOfStream)
            {
                string txt = reader.ReadLine();
                int id;
                if (Int32.TryParse(txt, out id))
                {
                    appearance.TakeItem(id);
                }
            }

            reader.Dispose();
            node.UpdateCharacter();
            SlotItems.GetAllSlotItems(node);
        }

        void Instance_MainM2SceneNodeChanged(object sender, M2SceneNode node)
        {
            Update();
        }

        void app_ApplicationLanguageChanged(object sender, Languages current)
        {
            Update();
        }

        public SlotItemCollection SlotItems
        {
            get { return _mainGrid.Resources["_slotItems"] as SlotItemCollection; }
        }

        public void Update()
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node == null)
                return;

            SlotItems.GetAllSlotItems(node);
        }

        private void btImport_Click(object sender, RoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node == null || node.Type != M2Type.MT_CHARACTER || node.IsNpc)
                return;

             M2Appearance appearance = node.M2Appearance;
             if (appearance == null)
                 return;

            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "text|*.txt|All Files|*.*";
            dlg.InitialDirectory = Engine.Instance.GetBaseDirectory();
            bool? ret = dlg.ShowDialog(ShellService.Instance.MainWindow);
            if (ret.HasValue && ret.Value)
            {
                StreamReader reader = new StreamReader(dlg.FileName);
               
                while(!reader.EndOfStream)
                {
                    string txt = reader.ReadLine();
                    int id;
                    if (Int32.TryParse(txt, out id))
                    {
                        appearance.TakeItem(id); 
                    }
                }

                reader.Dispose();

                ModelEditorService.Instance.ShowModelPart(node);
                ModelEditorService.Instance.ShowEditPart(node);
                ModelEditorService.Instance.SetWireFrame(node);
                ModelEditorService.Instance.SheathM2NodeWeapon(node);

                node.UpdateCharacter();

                //update property
                ShellService.Instance.PropertyContent.UpdateCharacterNode(node);
                Update();
            }
        }

        private void btExport_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "text|*.txt|All Files|*.*";
            dlg.InitialDirectory = Engine.Instance.GetBaseDirectory();
            bool? ret = dlg.ShowDialog(ShellService.Instance.MainWindow);
            if (ret.HasValue && ret.Value)
            {
                StreamWriter writer = new StreamWriter(dlg.FileName);
                foreach (SlotItem item in SlotItems)
                {
                    writer.WriteLine(item.ItemId);
                }
                writer.Dispose();
            }
        }

        private string IsSingleFile(DragEventArgs args)
        {
            // Check for files in the hovering data object.
            if (args.Data.GetDataPresent(DataFormats.FileDrop, true))
            {
                string[] fileNames = args.Data.GetData(DataFormats.FileDrop, true) as string[];
                // Check fo a single file or folder.
                if (fileNames.Length == 1)
                {
                    // Check for a file (a directory will return false).
                    if (File.Exists(fileNames[0]))
                    {
                        // At this point we know there is a single file.
                        return fileNames[0];
                    }
                }
            }
            return "";
        }

        private void btCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }

    }
}
