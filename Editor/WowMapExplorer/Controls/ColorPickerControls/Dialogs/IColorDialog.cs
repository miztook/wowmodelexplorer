using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media;

namespace ColorPickerControls.Dialogs
{
    interface IColorDialog
    {
        Color SelectedColor { get; set; }
        Color InitialColor { get; set; }
        bool? ShowDialog();
    }
}
