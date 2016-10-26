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

namespace WowModelExplorer.Controls
{
    /// <summary>
    /// Interaction logic for AccelerateWindow.xaml
    /// </summary>
    public partial class AccelerateWindow : Window
    {
        public AccelerateWindow()
        {
            InitializeComponent();
            this.Closing += (sender, e) => { e.Cancel = true; Hide(); };
        }

        private void btOk_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }
    }
}
