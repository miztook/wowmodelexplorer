using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Reflection;
using System.Windows;
using System.Windows.Media;

namespace FontDialog
{
    public static  class FontPropertyLists
    {
        static Collection<FontFamily> fontFaces = new Collection<FontFamily>();
        static Collection<FontStyle> fontStyles = new Collection<FontStyle>();
        static Collection<FontWeight> fontWeights = new Collection<FontWeight>();
        static Collection<double> fontSizes = new Collection<double>();

        static FontPropertyLists()
        {
            //family
            foreach (FontFamily fontFamily in Fonts.SystemFontFamilies)
            {
                fontFaces.Add(fontFamily);
            }

            //style
            fontStyles.Add(System.Windows.FontStyles.Normal);
            fontStyles.Add(System.Windows.FontStyles.Oblique);
            fontStyles.Add(System.Windows.FontStyles.Italic);

            //weights
            fontWeights.Add(System.Windows.FontWeights.Thin);
            fontWeights.Add(System.Windows.FontWeights.Light);
            fontWeights.Add(System.Windows.FontWeights.Normal);
            fontWeights.Add(System.Windows.FontWeights.Medium);
            fontWeights.Add(System.Windows.FontWeights.Heavy);
            fontWeights.Add(System.Windows.FontWeights.SemiBold);
            fontWeights.Add(System.Windows.FontWeights.Bold);
            fontWeights.Add(System.Windows.FontWeights.ExtraLight);
            fontWeights.Add(System.Windows.FontWeights.ExtraBold);
            fontWeights.Add(System.Windows.FontWeights.ExtraBlack);

            //sizes
            for (double i = 8; i <= 40; i++) fontSizes.Add(i);
        }

        /// <summary>
        /// Return a collection of avaliable font styles 
        /// </summary>
        public static ICollection<FontFamily> FontFaces
        {
            get
            {    
                return fontFaces;
            }
        }

        /// <summary>
        /// Return a collection of avaliable font styles 
        /// </summary>
        public static ICollection FontStyles
        {
            get
            {
                return fontStyles;
            }
        }

        /// <summary>
        /// Return a collection of avaliable FontWeight
        /// </summary>
        public static ICollection FontWeights
        {
            get
            {
                return fontWeights;
            }
        }

        /// <summary>
        /// Return a collection of font sizes.
        /// </summary>
        public static Collection<double> FontSizes
        {
            get
            {
                return fontSizes;
            }
        }

        public static bool CanParseFontStyle(string fontStyleName)
        {
            try
            {
                FontStyleConverter converter = new FontStyleConverter();
                converter.ConvertFromString(fontStyleName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        public static FontStyle ParseFontStyle(string fontStyleName)
        {
            FontStyleConverter converter = new FontStyleConverter();
            return (FontStyle)converter.ConvertFromString(fontStyleName);
        }
        public static bool CanParseFontWeight(string fontWeightName)
        {
            try
            {
                FontWeightConverter converter = new FontWeightConverter();
                converter.ConvertFromString(fontWeightName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        public static FontWeight ParseFontWeight(string fontWeightName)
        {
            FontWeightConverter converter = new FontWeightConverter();
            return (FontWeight)converter.ConvertFromString(fontWeightName);
        }
    }
}
