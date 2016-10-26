using System;
using System.Collections.Generic;
using System.ComponentModel;
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
using ColorPickerControls.Dialogs;
using ColorPicker;
namespace ColorPickerControls.Chips
{
    /// <summary>
    /// Interaction logic for ColorChip.xaml
    /// </summary>
    public partial class ColorChip : UserControl
    {

        public enum EMouseEvent
        {
            mouseDown,
            mouseUp,
            mouseDoubleClick
        }

        public event EventHandler<EventArgs<Color>> ColorChanged;

        public ColorChip()
        {
            InitializeComponent();
        }

        private EColorDialog mColorDialog = EColorDialog.Full;
        [Category("ColorPicker")]
        public EColorDialog ColorDialog
        {
            get { return mColorDialog; }
            set { mColorDialog = value; }
        }
 

        private EMouseEvent mDialogEvent = EMouseEvent.mouseDown;

        [Category("ColorPicker")]
        public EMouseEvent DialogEvent
        {
            get{ return mDialogEvent;}
            set { mDialogEvent = value; }
        }

        public static Type ClassType
        {
            get { return typeof(ColorChip); }
        }

        #region Color

        public static DependencyProperty ColorProperty = DependencyProperty.Register("Color", typeof(Color), ClassType, new PropertyMetadata(Colors.Gray, new PropertyChangedCallback(OnColorChanged)));
          [Category("ColorPicker")]
        public Color Color
        {
            get
            {
                return (Color)GetValue(ColorProperty);
            }
            set
            {
                SetValue(ColorProperty, value);
            }
        }

        private static void OnColorChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var chip = (ColorChip) d;
            var color = (Color) e.NewValue;
            chip.OnColorChanged(color);
        }

        private void OnColorChanged(Color color)
        {
            Brush = new SolidColorBrush(color);
            colorRect.Background =  Brush;
        }
        #endregion


        #region Brush

        public static DependencyProperty BrushProperty = DependencyProperty.Register("Brush", typeof(SolidColorBrush), ClassType, new PropertyMetadata(null, OnBrushChanged));

        public SolidColorBrush Brush
        {
            get
            {
                return (SolidColorBrush)GetValue(BrushProperty);
            }
            set
            {
                SetValue(BrushProperty, value);
            }
        }

        private static void OnBrushChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var chip = (ColorChip)d;
            var brush = (SolidColorBrush) e.NewValue;
            chip.Color = brush.Color;
        }

        #endregion





        private IColorDialog CreateDialog()
        {
            IColorDialog dialog = null;
            switch (ColorDialog)
            {
                case EColorDialog.Full:
                   dialog = new ColorPickerFullDialog();
                    break;
                case EColorDialog.FullWithAlpha:
                    dialog = new ColorPickerFullWithAlphaDialog();
                    break;
                case EColorDialog.Standard:
                    dialog = new ColorPickerStandardDialog();
                    break;
                case EColorDialog.StandardWithAlpha :
                    dialog = new ColorPickerStandardWithAlphaDialog();
                    break;
            }
            return dialog;
        }

        private void ShowDialog()
        {
            var dia =  CreateDialog();
            var initialColor =  ((SolidColorBrush)colorRect.Background).Color;
            dia.InitialColor = initialColor; //set the initial color
            if (dia.ShowDialog() == true)
            {
                if (dia.SelectedColor != initialColor)
                {
                    Color = dia.SelectedColor; //do something with the selected color
                    if (ColorChanged != null)
                    {
                        ColorChanged(this, new EventArgs<Color>(dia.SelectedColor));
                    }
                }
            }
        }

        private void UserControl_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (DialogEvent == EMouseEvent.mouseDown)
            {
                ShowDialog();
                e.Handled = true;
            }
        }

        private void UserControl_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (DialogEvent == EMouseEvent.mouseUp)
            {
                ShowDialog();
                e.Handled = true;
            }
        }

        private void UserControl_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (DialogEvent == EMouseEvent.mouseDoubleClick )
            {
                ShowDialog();
                e.Handled = true;
            }
        }
       



    }

}
