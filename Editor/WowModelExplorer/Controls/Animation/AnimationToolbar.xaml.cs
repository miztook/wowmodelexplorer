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

namespace WowModelExplorer.Controls.Animation
{
    /// <summary>
    /// Interaction logic for AnimationToolbar.xaml
    /// </summary>
    public partial class AnimationToolbar : UserControl
    {
        public AnimationToolbar()
        {
            InitializeComponent();
        }

        private bool loop = true;
        public bool Loop 
        {
            get { return loop; }
            set { loop = value; }
        }
    }
}
