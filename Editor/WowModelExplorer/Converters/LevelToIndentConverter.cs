using System;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Data;
using System.Globalization;

namespace WowModelExplorer.Converters
{
    public class LevelToIndentConverter : IValueConverter
    {
        public object Convert(object o, Type type, object parameter,
                              CultureInfo culture)
        {
            return new Thickness((int)o * c_IndentSize, 0, 0, 0);
        }

        public object ConvertBack(object o, Type type, object parameter,
                                  CultureInfo culture)
        {
            throw new NotSupportedException();
        }

        private const double c_IndentSize = 19.0;
    }
}
