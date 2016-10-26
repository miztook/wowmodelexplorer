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
using System.Windows.Controls.Primitives;

namespace WowModelExplorer.Controls.Animation
{
    /// <summary>
    /// Interaction logic for AnimationView.xaml
    /// </summary>
    public partial class AnimationView : UserControl
    {
        public AnimationView()
        {
            InitializeComponent();

            _animationHeader.FilterWidthChanged += new FilterWidthChangedEventHandler(_animationHeader_FilterWidthChanged);
            _animationHeader.filter.TextChanged += new TextChangedEventHandler(filter_TextChanged);

            _scrollbar.Scroll += new ScrollEventHandler(_scrollbar_Scroll);
        }

         //更新动画名字框宽度
        void _animationHeader_FilterWidthChanged(object sender, double newWidth)
        {
            for (int i = 0; i < _animationList.Items.Count; ++i)
            {
                ListBoxItem lbi = _animationList.Items[i] as ListBoxItem;
                AnimationEntry entry = lbi.Content as AnimationEntry;
                entry.Column0Width = newWidth;
            }
        }

        //滚动
        void _scrollbar_Scroll(object sender, ScrollEventArgs e)
        {
            _animationHeader.ScrollPosition = e.NewValue;
            for (int i = 0; i < _animationList.Items.Count; ++i)
            {
                ListBoxItem lbi = _animationList.Items[i] as ListBoxItem;
                AnimationEntry entry = lbi.Content as AnimationEntry;
                entry.ScrollPosition = e.NewValue;
            }
        }


        #region 计算可滚动宽度 
        //在动画加载后调用, 取动画长度的最大值
        public void RecalculateScrollWidth()
        {
            double m = 0;
            for (int i = 0; i < _animationList.Items.Count; ++i)
            {
                ListBoxItem lbi = _animationList.Items[i] as ListBoxItem;
                AnimationEntry entry = lbi.Content as AnimationEntry;
                if (m < entry.MinColumn1Width)
                {
                    m = entry.MinColumn1Width;
                }
            }
            
            _animationHeader._timelineColumn.MinWidth = m;
            _scrollbar.Maximum = m;
        }
#endregion

        #region 动画添加，清除，选中，更新进度

        public void ClearAll()
        {
            _animationList.Items.Clear();
            _animationHeader.filter.Text = "";
        }

        public void AddAnimation(string animName, uint animLength, uint animIndex)
        {
            ListBoxItem lbi = new ListBoxItem();

            AnimationEntry entry = new AnimationEntry()
            {
                Column0Width = _animationHeader.FilterWidth,
                AnimationIndex = animIndex,
                AnimationLength = animLength
            };
            entry.Register(lbi, _animationHeader);
            lbi.Content = entry;

            entry.AnimationName = animName;

            _animationList.Items.Add(lbi);
        }

        public int GetCurrentAnimationIndex()
        {
            ListBoxItem lbi = _animationList.SelectedItem as ListBoxItem;
            if (lbi == null)
                return -1;

            AnimationEntry entry = lbi.Content as AnimationEntry;
            return (int)entry.AnimationIndex;
        }

        public void SelectAnimation(int animIndex, bool loop)
        {
            foreach (ListBoxItem lbi in _animationList.Items)
            {
                AnimationEntry entry = lbi.Content as AnimationEntry;
                if (entry.AnimationIndex == animIndex)
                {
                    _animationToolbar.Loop = loop;
                    _animationList.SelectedItem = lbi;
                    _animationList.ScrollIntoView(lbi);
                    break;
                }
            }
        }

        public void UpdateProgress(int animIndex, float currentFrame)
        {
            _animationHeader.UpdateProgressIndicator(currentFrame);

            foreach (ListBoxItem lbi in _animationList.Items)
            {
                AnimationEntry entry = lbi.Content as AnimationEntry;
                if(entry.AnimationIndex == animIndex)
                {
                    entry.UpdateProgressIndicator(currentFrame);
                }
                else
                {
                    entry.UpdateProgressIndicator(0);
                }
            }
        }

#endregion

        #region 动画过滤
        void filter_TextChanged(object sender, TextChangedEventArgs e)
        {
            _animationList.Items.Filter = new System.Predicate<object>(IsNameMatch);
        }

        private bool IsNameMatch(object item)
        {
            ListBoxItem lbi = item as ListBoxItem;
            AnimationEntry i = lbi.Content as AnimationEntry;
            return -1 != i.AnimationName.IndexOf(_animationHeader.filter.Text, StringComparison.CurrentCultureIgnoreCase);
        }
#endregion

    }
}
