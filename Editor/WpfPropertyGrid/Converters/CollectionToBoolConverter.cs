using System;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Data;
using System.Globalization;
using System.Collections;
using System.Collections.Generic;

namespace System.Windows.Controls.WpfPropertyGrid
{
    public sealed class CollectionToBoolConverter : IValueConverter
    {
        public object Convert(object o, Type type, object parameter,
                              CultureInfo culture)
        {
            ICollection coll = o as ICollection;
            if (coll != null && coll.Count > 0)
                return true;
            return false;
        }

        public object ConvertBack(object o, Type type, object parameter,
                                  CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
