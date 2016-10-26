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
using WowModelExplorer.Services;

namespace WowModelExplorer.Controls.Animation
{
    public delegate void FilterWidthChangedEventHandler(object sender, double newWidth);
    public delegate void RangeChangedEventHandler(object sender, int newRange);

    /// <summary>
    /// Interaction logic for AnimationHeader.xaml
    /// </summary>
    public partial class AnimationHeader : UserControl
    {
        private double filterWidth;

        public AnimationHeader()
        {
            InitializeComponent();

            filter.SizeChanged += new SizeChangedEventHandler(filter_SizeChanged);
            timelineCanvas.SizeChanged += new SizeChangedEventHandler(timelineCanvas_SizeChanged);

            this.PreviewKeyDown += new KeyEventHandler(filter_PreviewKeyDown);
        }

        void filter_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (filter.IsKeyboardFocused)
                ShellService.Instance.MainWindow.IsKeyFocus = false;

            if (!filter.IsKeyboardFocused && e.Key == Key.W)
                e.Handled = true;
        }

        public event FilterWidthChangedEventHandler FilterWidthChanged;
        private void filter_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            filterWidth = e.NewSize.Width;
            if (FilterWidthChanged != null)
                FilterWidthChanged.Invoke(sender, filterWidth);
        }

        //刻度条尺寸改变时，重画刻度
        void timelineCanvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            UpdateTimeline();
        }

        public event RangeChangedEventHandler RangeChanged;
        public int Range
        {
            get { return range; }
            set
            {
                if (range != value)
                {
                    range = value;
                    UpdateTimeline();

                    if (RangeChanged != null)
                        RangeChanged.Invoke(this, range);
                }
            }
        }

        #region 画刻度

        private readonly Brush timeLineBrush = Brushes.White;
        private readonly List<Line> timeLineTicks = new List<Line>();
        private readonly Rectangle timelineBackgroundRegion = new Rectangle();
        private readonly List<TextBlock> timestampTextBlocks = new List<TextBlock>();

        private const int majorTickHeight = 10;
        private const int minorTickHeight = 4;

        private const int interval = 10;                   //每刻度10像素
        private int range = 50;           //每刻度表示大小

        public double FrameToWidth(double frame)
        {
            return frame * interval / range;
        }

        public double WidthToFrame(double width)
        {
            return width * range / interval;
        }

        private void UpdateTimeline()
        {
            foreach (TextBlock textblock in timestampTextBlocks)
            {
                timelineCanvas.Children.Remove(textblock);
            }
            timestampTextBlocks.Clear();

            foreach (Line line in timeLineTicks)
            {
                timelineCanvas.Children.Remove(line);
            }
            timeLineTicks.Clear();

            double bottomLoc = timelineCanvas.ActualHeight - 1;

            int numTicks = (int)timelineCanvas.ActualWidth / interval + 4;           //比最大动画长度稍长一点

            if (numTicks > 500)
                numTicks = 500;
            for (int i = 0; i <= numTicks; i++)
            {
                Line timelineTick = new Line()
                {
                    Stroke = timeLineBrush,
                    StrokeThickness = 1.0d
                };

                if (i % 4 == 0)
                {
                    double xLocation = (double)i * interval;
                    timelineTick.X1 = xLocation;
                    timelineTick.Y1 = bottomLoc;
                    timelineTick.X2 = xLocation;
                    timelineTick.Y2 = bottomLoc - majorTickHeight;

                    TextBlock timestampText = new TextBlock()
                    {
                        Margin = new Thickness(xLocation + 2, 0, 0, 0),
                        FontFamily = this.FontFamily,
                        FontStyle = this.FontStyle,
                        FontWeight = this.FontWeight,
                        FontStretch = this.FontStretch,
                        FontSize = this.FontSize,
                        Foreground = timeLineBrush,
                        Text = string.Format("{0}", i * range)
                    };
                    timestampTextBlocks.Add(timestampText);
                    timelineCanvas.Children.Add(timestampText);
                    UpdateLayout(); // Needed so that we know the width of the textblock.
                }
                else
                {
                    double xLocation = (double)i * interval;
                    timelineTick.X1 = xLocation;
                    timelineTick.Y1 = bottomLoc;
                    timelineTick.X2 = xLocation;
                    timelineTick.Y2 = bottomLoc - minorTickHeight;
                }
                timeLineTicks.Add(timelineTick);
                timelineCanvas.Children.Add(timelineTick);
            }
        }
        #endregion

        public double FilterWidth { get { return filterWidth; } }

        public double ScrollPosition { set { timelineCanvas.Margin = new System.Windows.Thickness(-value, 0, value, 0); } }

        #region 动画播放时, 更新刻度条上的光标
        //Progress Indicator

        float lastFrame = -1.0f;
        public void UpdateProgressIndicator(float currentFrame)
        {
            if (currentFrame != lastFrame)
            {
                double xLocation = 0.0d;
                xLocation = FrameToWidth(currentFrame);
                _progressRectangle.Margin = new Thickness(xLocation - 1, 0, 0, 0);
            }
            lastFrame = currentFrame;
        }
        #endregion

    }
}
