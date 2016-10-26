using System;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Data;
using System.Globalization;
using System.Windows.Media;

namespace FontDialog.Converters
{
    public class FontFamilyConverter : IValueConverter
    {
        public object Convert(object o, Type type, object parameter,
                              CultureInfo culture)
        {
            if (type == typeof(FontFamily) && o != null)
                return o;
            else
                return new FontFamily();
        }

        public object ConvertBack(object o, Type type, object parameter,
                                  CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class FontStyleConverter : IValueConverter
    {
        public object Convert(object o, Type type, object parameter,
                              CultureInfo culture)
        {
            if (type == typeof(FontStyle) && o != null)
                return o;
            else
                return FontStyles.Normal;
        }

        public object ConvertBack(object o, Type type, object parameter,
                                  CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class FontWeightConverter : IValueConverter
    {
        public object Convert(object o, Type type, object parameter,
                              CultureInfo culture)
        {
            if (type == typeof(FontWeight) && o != null)
                return o;
            else
                return FontWeights.Normal;
        }

        public object ConvertBack(object o, Type type, object parameter,
                                  CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
