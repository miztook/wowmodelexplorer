using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using WowModelExplorer.Data;
using mywowNet;
using mywowNet.Data;
using WowModelExplorer.Services;

namespace WowModelExplorer.Controls
{
    public delegate void M2State_SelectedEventHandler(object sender, M2State state);

    /// <summary>
    /// Interaction logic for StateSelect.xaml
    /// </summary>
    public partial class StateSelect : UserControl
    {
        private ICollectionView statesView;

        public StateSelect()
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

        public void Initialize()
        {
            App app = Application.Current as App;
            app.ApplicationLanguageChanged += new Application_LanguageChangedHandler(app_ApplicationLanguageChanged);
        }

        void app_ApplicationLanguageChanged(object sender, Languages current)
        {
            if (statesView != null)
                statesView.Filter = null;

            States.Clear();

            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null && node.M2Fsm != null)
            {
                for (int i = 0; i < (int)E_M2_STATES.EMS_MOUNT; ++i)
                {
                    if (node.M2Fsm.IsStateValid((E_M2_STATES)i))
                    {
                        M2State s = new M2State();
                        s.State = (E_M2_STATES)i;
                        s.Name = s.GetStateName(current);
                        States.Add(s);
                    }
                }
            }
            
            statesView = CollectionViewSource.GetDefaultView(States);

            FilterState();
        }

        public void FillData(M2SceneNode node)
        {
            App app = Application.Current as App;

            States.Clear();
            if (node != null && node.M2Fsm != null)
            {
                for (int i = 0; i < (int)E_M2_STATES.EMS_MOUNT; ++i)
                {
                    if (node.M2Fsm.IsStateValid((E_M2_STATES)i))
                    {
                        M2State s = new M2State();
                        s.State = (E_M2_STATES)i;
                        s.Name = s.GetStateName(app.Language);
                        States.Add(s);
                    }
                }
            }
           
            statesView = CollectionViewSource.GetDefaultView(States);
        }

        public M2StateCollection States
        {
            get { return _mainGrid.Resources["_states"] as M2StateCollection; }
        }

        private bool IsMatch(object state)
        {
            M2State i = state as M2State;

            return -1 != i.Name.IndexOf(filter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void filter_TextChanged(object sender, TextChangedEventArgs e)
        {
            FilterState();
        }

        private void FilterState()
        {
            if (statesView == null)
                return;

            statesView.Filter = new System.Predicate<object>(IsMatch);
        }

        bool nameAscending = true;
        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (statesView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            statesView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Name":
                    {
                        statesView.SortDescriptions.Add(new SortDescription("Name",
                                                nameAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        nameAscending = !nameAscending;
                    }
                    break;
            }
        }

        public event M2State_SelectedEventHandler State_Selected;

        private void listViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            ChangeState();
        }

        private void listView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                ChangeState();
        }

        private void ChangeState()
        {
            if (Client.Instance.GetPlayer().GetRideNpcId() != 0)
                return;

            M2State state = _listView.SelectedItem as M2State;
            if (state != null)
            {
                if (State_Selected != null)
                    State_Selected.Invoke(this, state);
            }
        }
    }
}
