using System;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Data;
using System.Globalization;

namespace System.Windows.Controls.WpfPropertyGrid
{
    public sealed class ArrayLengthConverter : IValueConverter
    {
        public object Convert(object o, Type type, object parameter,
                              CultureInfo culture)
        {
            Array arr = o as Array;
            if (arr != null)
            {
                return String.Format("count: {0}", arr.Length);
            }
            return String.Format("0");
        }

        public object ConvertBack(object o, Type type, object parameter,
                                  CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
