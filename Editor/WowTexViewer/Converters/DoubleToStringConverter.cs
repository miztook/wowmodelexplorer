using System;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Data;
using System.Globalization;

namespace WowTexViewer.Converters
{
    public class DoubleToStringConverter : IValueConverter
    {
        public object Convert(object o, Type type, object parameter,
                              CultureInfo culture)
        {
            double d = (double)o;
            return string.Format("{0:F2}", d);
        }

        public object ConvertBack(object o, Type type, object parameter,
                                  CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}