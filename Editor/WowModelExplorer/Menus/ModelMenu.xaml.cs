using System;
using System.Windows;
using System.Windows.Controls;
using WowModelExplorer.Services;
using AvalonDock;
using mywowNet;

namespace WowModelExplorer.Menus
{
    /// <summary>
    /// Interaction logic for ModelMenu.xaml
    /// </summary>
    public partial class ModelMenu : UserControl
    {
        public ModelMenu()
        {
            InitializeComponent();

            this.Loaded += new RoutedEventHandler(ModelMenu_Loaded);
        }

        void ModelMenu_Loaded(object sender, RoutedEventArgs e)
        {
            m2SliderScale.ValueChanged += new RoutedPropertyChangedEventHandler<double>(M2SliderScale_ValueChanged);
            m2SliderOpacity.ValueChanged += new RoutedPropertyChangedEventHandler<double>(M2SliderOpacity_ValueChanged);
            wmoSliderScale.ValueChanged += new RoutedPropertyChangedEventHandler<double>(wmoSliderScale_ValueChanged);
        }

        private void menuView_SubmenuOpened(object sender, RoutedEventArgs e)
        {
#if !WOW60 && !WOW50 && !WOW40 && !WOW30
            MenuItem[] menuItems = new MenuItem[]
            {
                menuViewCharacter,
                menuViewClothes,
                menuViewWeapon,
                menuViewSet,
                menuViewRidable,

                menuOperationAttachments,
                menuOperationEquipments,
                menuOperationCharacterInfo,
            };

            foreach(var item in menuItems)
            {
                item.IsEnabled = false;
            }
#else

            menuViewCharacter.IsChecked = ShellService.Instance.CharacterContent.State != DockableContentState.Hidden;
            menuViewNpc.IsChecked = ShellService.Instance.NpcContent.State != DockableContentState.Hidden;
            menuViewSpellVisualEffect.IsChecked = ShellService.Instance.SpellVisualEffectContent.State != DockableContentState.Hidden;
            //menuViewMapModel.IsChecked = ShellService.Instance.MapModelContent.State != DockableContentState.Hidden;
            menuViewWmo.IsChecked = ShellService.Instance.WmoContent.State != DockableContentState.Hidden;
            menuViewWorldM2.IsChecked = ShellService.Instance.WorldM2Content.State != DockableContentState.Hidden;
            menuViewClothes.IsChecked = ShellService.Instance.ClothesContent.State != DockableContentState.Hidden;
            menuViewWeapon.IsChecked = ShellService.Instance.WeaponContent.State != DockableContentState.Hidden;
            menuViewSet.IsChecked = ShellService.Instance.SetContent.State != DockableContentState.Hidden;
            menuViewRidable.IsChecked = ShellService.Instance.RidableContent.State != DockableContentState.Hidden;
            //menuViewMapModelName.IsChecked = ShellService.Instance.MapModelNameContent.State != DockableContentState.Hidden;
            menuViewProperty.IsChecked = ShellService.Instance.PropertyContent.State != DockableContentState.Hidden;
            menuViewAnimation.IsChecked = ShellService.Instance.AnimationContent.State != DockableContentState.Hidden;
            menuViewM2State.IsChecked = ShellService.Instance.M2StateContent.State != DockableContentState.Hidden;
#endif
        }

        private void MenuOperation_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuOperationGeosets.IsChecked = ShellService.Instance.GeosetsWindow.IsVisible;
            menuOperationAttachments.IsChecked = ShellService.Instance.AttachmentsWindow.IsVisible;
            menuOperationEquipments.IsChecked = ShellService.Instance.EquipmentsWindow.IsVisible;
            //menuOperationEquipmentSelect.IsChecked = ShellService.Instance.EquipmentSelectWindow.IsVisible;
            menuOperationCharacterInfo.IsChecked = ShellService.Instance.CharacterInfoWindow.IsVisible;
          //  menuOperationArmory.IsChecked = ShellService.Instance.ArmoryWindow.IsVisible;
        }

        private void ShowOtherModel_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuShowHelm.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_PART.EMP_HEAD);
            menuShowCape.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_PART.EMP_CAPE);
            menuShowShoulder.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_PART.EMP_SHOULDER);
            menuShowLeftHand.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_PART.EMP_LEFTHAND);
            menuShowRightHand.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_PART.EMP_RIGHTHAND);
        }

        private void menuEditView_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuShowModel.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_EDIT_PART.EEP_MODEL);
            menuShowParticles.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_EDIT_PART.EEP_PARTICLES);
            menuShowBoundingBox.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_EDIT_PART.EEP_BOUNDINGBOX);
            menuShowBones.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_EDIT_PART.EEP_BONES);
            menuShowBoundingAABox.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_EDIT_PART.EEP_BOUNDINGAABOX);
            menuShowCollisionAABox.IsChecked = ModelEditorService.Instance.IsShow(E_MODEL_EDIT_PART.EEP_COLLISIONAABOX);
            menuModelStateMode.IsChecked = ModelEditorService.Instance.StateMode;
            menuModelSheathWeapon.IsChecked = ModelEditorService.Instance.SheathWeapon;
        }

        private void menuWmoView_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuWmoShowWMO.IsChecked = ModelEditorService.Instance.IsShow(E_WMO_EDIT_PART.EEP_WMO);
        }

        private void menuEditorOption_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuOptionAccelerate.IsChecked = ShellService.Instance.AccelerateWindow.IsVisible;
        }

        private void menuOptionSettings_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuOptionLineZTest.IsChecked = Engine.Instance.DrawServices.LineZTestEnable;
        }

        private void menuOptionShowHide_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuOptionShowBaseInfo.IsChecked = ModelEditorService.Instance.IsShow(EditorShowOptions.DebugText);
            menuOptionShowGeosets.IsChecked = ModelEditorService.Instance.IsShow(EditorShowOptions.DebugGeosets);
            menuOptionShowPlane.IsChecked = ModelEditorService.Instance.IsShow(EditorShowOptions.Plane);
            menuOptionShowCoord.IsChecked = ModelEditorService.Instance.IsShow(EditorShowOptions.Coord);
            menuOptionShowLightCoord.IsChecked = ModelEditorService.Instance.IsShow(EditorShowOptions.LightCoord);
        }

        private void menuModelWireframe_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuModelWireFrameNone.IsChecked = ModelEditorService.Instance.M2WireFrame == E_OVERRIDE_WIREFRAME.EOW_NONE;
            menuModelWireFrameOnly.IsChecked = ModelEditorService.Instance.M2WireFrame == E_OVERRIDE_WIREFRAME.EOW_WIREFRAME;
            menuModelWireFrameSolid.IsChecked = ModelEditorService.Instance.M2WireFrame == E_OVERRIDE_WIREFRAME.EOW_WIREFRAME_SOLID;
            menuModelWireFrameOneColor.IsChecked = ModelEditorService.Instance.M2WireFrame == E_OVERRIDE_WIREFRAME.EOW_WIREFRAME_ONECOLOR;
        }

        private void menuWmoWireframe_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuWmoWireFrameNone.IsChecked = ModelEditorService.Instance.WmoWireFrame == E_OVERRIDE_WIREFRAME.EOW_NONE;
            menuWmoWireFrameOnly.IsChecked = ModelEditorService.Instance.WmoWireFrame == E_OVERRIDE_WIREFRAME.EOW_WIREFRAME;
            menuWmoWireFrameSolid.IsChecked = ModelEditorService.Instance.WmoWireFrame == E_OVERRIDE_WIREFRAME.EOW_WIREFRAME_SOLID;
            menuWmoWireFrameOneColor.IsChecked = ModelEditorService.Instance.WmoWireFrame == E_OVERRIDE_WIREFRAME.EOW_WIREFRAME_ONECOLOR;
        }

        /*
        private void menuOptionBackMusic_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuOptionBackMusicNone.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.None;
        }

        private void menuThemeMusic_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuOptionBackMusicMainTiltle.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.MainTitle;
            menuOptionBackMusicWotLK.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.WotLK;
            menuOptionBackMusicCataclysm.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.Cataclysm;
            menuOptionBackMusicPandaria.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.Pandaria;
            menuOptionBackMusicDraenor.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.Draenor;
            menuOptionBackMusicHighBorne.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.HighBorne;
            menuOptionBackMusicGarrosh.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.Garrosh;
        }

        private void menuCityMusic_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuOptionBackMusicStormwind.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.Stormwind;
            menuOptionBackMusicDarnassus.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.Darnassus;
            menuOptionBackMusicIronforge.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.Ironforge;
            menuOptionBackMusicOrgrimmar.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.Orgrimmar;
            menuOptionBackMusicUndercity.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.Undercity;
            menuOptionBackMusicThunderbluff.IsChecked = ModelEditorService.Instance.BackMusic == BackMusicFiles.Thunderbluff;
        }
         */

        private void menuSettingCameraSpeed_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuSettingCameraSpeedLowest.IsChecked = EngineService.Instance.KeySpeed == 0.05f;
            menuSettingCameraSpeedLow.IsChecked = EngineService.Instance.KeySpeed == 0.1f;
            menuSettingCameraSpeedMedium.IsChecked = EngineService.Instance.KeySpeed == 0.3f;
            menuSettingCameraSpeedHigh.IsChecked = EngineService.Instance.KeySpeed == 0.8f;
            menuSettingCameraSpeedHighest.IsChecked = EngineService.Instance.KeySpeed == 2.0f;
        }

        private void menuSettingCameraModes_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuSettingCameraModesFPS.IsChecked = EngineService.Instance.CameraMode == CameraModes.FPS;
            menuSettingCameraModesMaya.IsChecked = EngineService.Instance.CameraMode == CameraModes.Maya;
        }

        private void menuSettingWheelSpeed_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuSettingWheelSpeedOff.IsChecked = EngineService.Instance.WheelSpeed == 0.0f;
            menuSettingWheelSpeedLowest.IsChecked = EngineService.Instance.WheelSpeed == 0.05f;
            menuSettingWheelSpeedLow.IsChecked = EngineService.Instance.WheelSpeed == 0.25f;
            menuSettingWheelSpeedMedium.IsChecked = EngineService.Instance.WheelSpeed == 0.5f;
            menuSettingWheelSpeedHigh.IsChecked = EngineService.Instance.WheelSpeed == 2.5f;
            menuSettingWheelSpeedHighest.IsChecked = EngineService.Instance.WheelSpeed == 5.0f;
        }

        private void menuEngineOption_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuSettingGameMode.IsChecked = EngineService.Instance.IsGameMode;
        }

        private void menuLanguages_SubmenuOpend(object sender, RoutedEventArgs e)
        {
            App app = Application.Current as App;
            menuLanguagesChinese.IsChecked = app.Language == Languages.Chinese;
            menuLanguagesEnglish.IsChecked = app.Language == Languages.English;
        }

        private void menuModelScale_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                matrix4 mat = new matrix4(true);
                node.GetRelativeTransformation(out mat);
                m2SliderScale.Value = mat.Scale.X;
            }
        }

        private void menuModelOpacity_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                m2SliderOpacity.Value = node.ModelAlpha;
            }
        }

        private void M2SliderScale_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                float scale = (float)e.NewValue;

                M2Move move =  node.M2Move;
                if (move != null)
                {
                    move.Scale = new vector3df(scale, scale, scale);
                } 
                else
                {
                    matrix4 mat = new matrix4(true);
                    node.GetRelativeTransformation(out mat);

                    matrix4 newMat = new matrix4(true);
                    newMat.RotationRadians = mat.RotationRadians;
                    newMat.Scale = new vector3df(scale, scale, scale);
                    newMat.Translation = mat.Translation;
                    node.SetRelativeTransformation(ref newMat);
                }
              
                node.Update(true);
            }
        }

        private void M2SliderOpacity_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                node.ModelAlpha = (float)e.NewValue;
            }
        }

        private void M2ScaleDefault_Click(object sender, RoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                float scale = 1.0f;

                M2Move move =  node.M2Move;
                if (move != null)
                {
                    move.Scale = new vector3df(scale, scale, scale);
                } 
                else
                {
                    matrix4 mat = new matrix4(true);
                    node.GetRelativeTransformation(out mat);

                    matrix4 newMat = new matrix4(true);
                    newMat.RotationRadians = mat.RotationRadians;
                    newMat.Scale = new vector3df(scale, scale, scale);
                    newMat.Translation = mat.Translation;
                    node.SetRelativeTransformation(ref newMat);
                }
              
                node.Update(true);
            }
        }

        private void M2OpacityDefault_Click(object sender, RoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                node.ModelAlpha = 1.0f;
            }
        }

        private void menuWmoOperation_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuWmoGroups.IsChecked = ShellService.Instance.GroupsWindow.IsVisible;
            menuWmoPortals.IsChecked = ShellService.Instance.PortalsWindow.IsVisible;
        }

        private void menuWmoScale_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
            if (node != null)
            {
                matrix4 mat = new matrix4(true);
                node.GetRelativeTransformation(out mat);
                wmoSliderScale.Value = mat.Scale.X;
            }
        }

        void wmoSliderScale_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
            if (node != null)
            {
                float scale = (float)e.NewValue;
                matrix4 mat = new matrix4(true);
                node.GetRelativeTransformation(out mat);

                matrix4 newMat = new matrix4(true);
                newMat.RotationRadians = mat.RotationRadians;
                newMat.Scale = new vector3df(scale, scale, scale);
                newMat.Translation = mat.Translation;
                node.SetRelativeTransformation(ref newMat);
                node.Update(true);
            }
        }

        private void WmoScaleDefault_Click(object sender, RoutedEventArgs e)
        {
            WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
            if (node != null)
            {
                float scale = 3.0f;
                matrix4 mat = new matrix4(true);
                node.GetRelativeTransformation(out mat);

                matrix4 newMat = new matrix4(true);
                newMat.RotationRadians = mat.RotationRadians;
                newMat.Scale = new vector3df(scale, scale, scale);
                newMat.Translation = mat.Translation;
                node.SetRelativeTransformation(ref newMat);
                node.Update(true);
            }
        }



    }
}
