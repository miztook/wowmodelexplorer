using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using WowModelExplorer.Data;
using WowModelExplorer.Services;
using mywowNet.Data;

namespace WowModelExplorer.Controls
{
    public delegate void SpellVisualEffect_SelectedEventHandler(object sender, SpellVisualEffect visual);

    /// <summary>
    /// Interaction logic for SpellVisualEffectSelect.xaml
    /// </summary>
    public partial class SpellVisualEffectSelect : UserControl
    {
        private ICollectionView spellVisualEffectsView;

        public SpellVisualEffectSelect()
        {
            InitializeComponent();

            _listView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(listViewItemDbClicked));
            _listView.KeyUp += new KeyEventHandler(listView_KeyUp);
            this.PreviewKeyDown += new KeyEventHandler(filter_PreviewKeyDown);
        }

        void filter_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (filter.IsKeyboardFocused)
                ShellService.Instance.MainWindow.IsKeyFocus = false;

            if (!filter.IsKeyboardFocused && e.Key == Key.W)
                e.Handled = true;
        }

        public void FillData()
        {
            SpellVisualEffects.GetAllSpellVisualEffects();
            spellVisualEffectsView = CollectionViewSource.GetDefaultView(SpellVisualEffects);
        }

        public SpellVisualEffectCollection SpellVisualEffects
        {
            get { return _mainGrid.Resources["_spellVisualEffects"] as SpellVisualEffectCollection; }
        }

        private bool IsMatch(object visual)
        {
            SpellVisualEffect i = visual as SpellVisualEffect;

            return -1 != i.Name.IndexOf(filter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void filter_TextChanged(object sender, TextChangedEventArgs e)
        {
            FilterSpellVisualEffect();
        }

        private void FilterSpellVisualEffect()
        {
            if (spellVisualEffectsView == null)
                return;

            spellVisualEffectsView.Filter = new System.Predicate<object>(IsMatch);
        }

        bool nameAscending = true;
        bool idAscending = true;
        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (spellVisualEffectsView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            spellVisualEffectsView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Name":
                    {
                        spellVisualEffectsView.SortDescriptions.Add(new SortDescription("Name",
                                                nameAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        nameAscending = !nameAscending;
                    }
                    break;
                case "Id":
                    {
                        spellVisualEffectsView.SortDescriptions.Add(new SortDescription("Id",
                                               idAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        idAscending = !idAscending;
                    }
                    break;
            }
        }

        public event SpellVisualEffect_SelectedEventHandler SpellVisualEffect_Selected;

        private void listViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadSpellVisualEffect();
        }

        private void listView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadSpellVisualEffect();
        }

        private void LoadSpellVisualEffect()
        {
            SpellVisualEffect visual = _listView.SelectedItem as SpellVisualEffect;
            if (visual != null)
            {
                if (SpellVisualEffect_Selected != null)
                    SpellVisualEffect_Selected.Invoke(this, visual);
            }
        }

    }
}
