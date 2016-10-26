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

namespace WowModelExplorer.Controls.Common
{
    /// <summary>
    /// Interaction logic for LabelEntry.xaml
    /// </summary>
    public partial class LabelEntry : UserControl
    {
        public LabelEntry()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty DescriptionProperty =
         DependencyProperty.Register("Description", typeof(string),
         typeof(LabelEntry),
         new PropertyMetadata(new PropertyChangedCallback(OnDescriptionChanged)));

        private static void OnDescriptionChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            LabelEntry control = (LabelEntry)obj;
            control._desc.Text = (string)args.NewValue;
        }

        public string Description
        {
            get { return (string)GetValue(DescriptionProperty); }
            set { SetValue(DescriptionProperty, value); }
        }

        public static readonly DependencyProperty TextProperty =
        DependencyProperty.Register("Text", typeof(string),
        typeof(LabelEntry),
        new PropertyMetadata(new PropertyChangedCallback(OnTextChanged)));

        private static void OnTextChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            LabelEntry control = (LabelEntry)obj;
            control._text.Text = (string)args.NewValue;
        }

        public string Text
        {
            get { return (string)GetValue(TextProperty); }
            set { SetValue(TextProperty, value); }
        }
    }
}
