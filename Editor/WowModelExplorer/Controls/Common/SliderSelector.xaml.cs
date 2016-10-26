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
    /// Interaction logic for SliderSelector.xaml
    /// </summary>
    public partial class SliderSelector : UserControl
    {
        public SliderSelector()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty DescriptionProperty =
         DependencyProperty.Register("Description", typeof(string),
         typeof(SliderSelector),
         new PropertyMetadata(new PropertyChangedCallback(OnDescriptionChanged)));

        private static void OnDescriptionChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            SliderSelector control = (SliderSelector)obj;
            control._desc.Text = (string)args.NewValue;
        }

        public string Description
        {
            get { return (string)GetValue(DescriptionProperty); }
            set { SetValue(DescriptionProperty, value); }
        }

        public static readonly DependencyProperty MaximumProperty =
         DependencyProperty.Register("Maximum", typeof(uint),
         typeof(SliderSelector),
         new PropertyMetadata(new PropertyChangedCallback(OnMaximumChanged)));

        private static void OnMaximumChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            SliderSelector control = (SliderSelector)obj;
            control._scrollbar.Maximum = (double)((uint)args.NewValue);
            control.UpdateText();
        }

        public uint Maximum
        {
            get { return (uint)GetValue(MaximumProperty); }
            set { SetValue(MaximumProperty, value); }
        }

        public static readonly DependencyProperty ValueProperty =
         DependencyProperty.Register("Value", typeof(uint),
         typeof(SliderSelector),
         new PropertyMetadata(new PropertyChangedCallback(OnValueChanged)));

        private static void OnValueChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            SliderSelector control = (SliderSelector)obj;
            control._scrollbar.Value = (double)((uint)args.NewValue);
        }

        public uint Value
        {
            get { return (uint)GetValue(ValueProperty); }
            set { SetValue(ValueProperty, value); }
        }

        public static readonly RoutedEvent ValueChangedEvent = EventManager.RegisterRoutedEvent(
          "ValueChanged", RoutingStrategy.Bubble,
          typeof(RoutedPropertyChangedEventHandler<int>), typeof(SliderSelector));

        public event RoutedPropertyChangedEventHandler<int> ValueChanged
        {
            add { AddHandler(ValueChangedEvent, value); }
            remove { RemoveHandler(ValueChangedEvent, value); }
        }

        private void _scrollbar_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            Value = (uint)e.NewValue;
            UpdateText();

            RoutedPropertyChangedEventArgs<int> arg = new RoutedPropertyChangedEventArgs<int>((int)e.OldValue, (int)e.NewValue);
            arg.RoutedEvent = ValueChangedEvent;

            RaiseEvent(arg);
        }

        private void UpdateText()
        {
            _text.Text = string.Format("{0}/{1}", (int)_scrollbar.Value, Maximum);
        }
    }
}
