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
using System.Windows.Shapes;
using WowModelExplorer.Props;
using WowModelExplorer.Services;
using WowModelExplorer.Views;

namespace WowModelExplorer.Controls
{
    /// <summary>
    /// Interaction logic for AnimationConfigWindow.xaml
    /// </summary>
    public partial class AnimationConfigWindow : Window
    {
        public AnimationConfigWindow()
        {
            InitializeComponent();

            Initialize();
        }

        public string Range
        {
            get;
            set;
        }

        void Initialize()
        {
            Range = ShellService.Instance.AnimationContent._animationSelect._animationHeader.Range.ToString();
            _tbRange.Focus();
        }

        private void btOk_Click(object sender, RoutedEventArgs e)
        {
            BindingExpression be = _tbRange.GetBindingExpression(TextBox.TextProperty);
            be.UpdateSource();

            int num;
            if(Int32.TryParse(Range, out num) && num > 0)
                ShellService.Instance.AnimationContent._animationSelect._animationHeader.Range = num;

            Close();
        }
    }
}
