using System;
using System.Windows;
using System.Windows.Controls.WpfPropertyGrid;

namespace WowModelExplorer.PropEditors
{
    public class TextureCollectionEditor : PropertyEditor
    {
        public TextureCollectionEditor()
        {
            this.InlineTemplate = EditorKeys.TextureCollectionKey;
        }

        public override void ShowDialog(PropertyItemValue propertyValue, IInputElement commandSource)
        {
            if (propertyValue == null)
                return;
            if (!propertyValue.IsCollection)
                return;

            MessageBox.Show("Collection!");
        }

    }
}
