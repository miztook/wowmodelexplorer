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
    /// Interaction logic for AnimationEntry.xaml
    /// </summary>
    public partial class AnimationEntry : UserControl
    {    
        public AnimationEntry()
        {
            InitializeComponent();
            MakeAppearance(false);
        }

        private AnimationHeader animationHeader;
        private ListBoxItem listboxitem;
        public void Register(ListBoxItem item, AnimationHeader header)
        {
            animationHeader = header;
            _animCanvas.Width = animationHeader.FrameToWidth(AnimationLength);

            listboxitem = item;
            listboxitem.Selected += new RoutedEventHandler(listboxitem_Selected);
            listboxitem.Unselected += new RoutedEventHandler(listboxitem_Unselected);

            animationHeader.RangeChanged += (sender, range) =>
            { _animCanvas.Width = animationHeader.FrameToWidth(AnimationLength); };
        }

        private void listboxitem_Selected(object sender, RoutedEventArgs e)
        {
            MakeAppearance(true);
        }

        private void listboxitem_Unselected(object sender, RoutedEventArgs e)
        {
            MakeAppearance(false);
        }

        private void MakeAppearance(bool selected)
        {
            if (selected)
            {
                _name.Foreground = Brushes.Black;
                _animCanvas.Background = Brushes.Orange;
            }
            else
            {
                _name.Foreground = Brushes.White;
                _animCanvas.Background = Brushes.LightBlue;
            }
        }

        #region 动画数据
        public uint AnimationIndex
        {
            get;
            set;
        }

        public uint AnimationLength
        {
            get;
            set;
        }

        private string _animationName;
        public string AnimationName
        {
            get { return _animationName; }
            set
            {
                _animationName = value;
                _name.Text = _animationName;
            }
        }
        #endregion

        #region 滚动,宽度设置

        //动画名宽度，随AnimationHeader的column0宽度变化
        public double Column0Width
        {
            get { return _column0.Width.Value; }
            set
            {
                _column0.Width = new GridLength(value);
            }
        }

        //动画的实际长度
        public double MinColumn1Width
        {
            get { return _animCanvas.Width; }
        }

        //位置滚动的位置
        public double ScrollPosition
        {
            set
            {
                _animCanvas.Margin = new System.Windows.Thickness(-value, 0, value, 0);
            }
        }

        #endregion

        #region 更新进度

        float lastFrame = -1.0f;
        public void UpdateProgressIndicator(float currentFrame)
        {
            if (currentFrame != lastFrame)
            {
                double xLocation = 0.0d;
                xLocation = animationHeader.FrameToWidth(currentFrame);
                _progressRectangle.Width = xLocation;
            }
            lastFrame = currentFrame;
        }
        #endregion

    }
}
