using System;
using System.Globalization;
using System.Windows.Data;

namespace System.Windows.Controls.WpfPropertyGrid
{
    public sealed class StringToNullConverter : IValueConverter
    {
        public object Convert(object o, Type type, object parameter,
                             CultureInfo culture)
        {
            String str = o as String;
            if (str != null && str.Length > 0)
                return str;
            return null;
        }

        public object ConvertBack(object o, Type type, object parameter,
                                  CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
