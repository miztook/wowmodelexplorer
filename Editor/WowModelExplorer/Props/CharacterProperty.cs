using System;
using System.Windows;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using mywowNet;
using System.Windows.Controls.WpfPropertyGrid;
using WowModelExplorer.PropEditors;
using System.ComponentModel;

namespace WowModelExplorer.Props
{
    public class ComboTextureProperty : INotifyPropertyChanged
    {
#region display name
        public const string propBase = "身体";
        public const string propArmUpper = "手臂上";
        public const string propArmLower = "手臂下";
        public const string propHand = "手";
        public const string propFaceUpper = "脸上";
        public const string propFaceLower = "脸下";
        public const string propTorsoUpper = "躯干上";
        public const string propTorsoLower = "躯干下";
        public const string propLegUpper = "腿上";
        public const string propLegLower = "腿下";
        public const string propFoot = "脚";
        public const string displayName = "组合纹理";

#endregion

        private string baseName;
        private string armUpperName;
        private string armLowerName;
        private string handName;
        private string faceUpperName;
        private string faceLowerName;
        private string torsoUppderName;
        private string torsoLowerName;
        private string legUpperName;
        private string legLowerName;
        private string footName;

        public void FromM2SceneNode(M2SceneNode node)
        {
            BaseName = node.GetRegionTextureFileName(ECharRegions.CR_BASE);
            ArmUpperName = node.GetRegionTextureFileName(ECharRegions.CR_ARM_UPPER);
            ArmLowerName = node.GetRegionTextureFileName(ECharRegions.CR_ARM_LOWER);
            HandName = node.GetRegionTextureFileName(ECharRegions.CR_HAND);
            FaceUpperName = node.GetRegionTextureFileName(ECharRegions.CR_FACE_UPPER);
            FaceLowerName = node.GetRegionTextureFileName(ECharRegions.CR_FACE_LOWER);
            TorsoUpperName = node.GetRegionTextureFileName(ECharRegions.CR_TORSO_UPPER);
            TorsoLowerName = node.GetRegionTextureFileName(ECharRegions.CR_TORSO_LOWER);
            LegUpperName = node.GetRegionTextureFileName(ECharRegions.CR_LEG_UPPER);
            LegLowerName = node.GetRegionTextureFileName(ECharRegions.CR_LEG_LOWER);
            FootName = node.GetRegionTextureFileName(ECharRegions.CR_FOOT);
        }

        public override string ToString()
        {
            return displayName;
        }

        #region Properties

        [ReadOnly(true)]
        [DisplayName(propBase)]
        [PropertyOrder(0)]
        public string BaseName
        {
            get { return baseName; }
            set
            {
                if (baseName == value) return;
                baseName = value;
                OnPropertyChanged("BaseName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propArmUpper)]
        [PropertyOrder(1)]
        public string ArmUpperName
        {
            get { return armUpperName; }
            set
            {
                if (armUpperName == value) return;
                armUpperName = value;
                OnPropertyChanged("ArmUpperName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propArmLower)]
        [PropertyOrder(2)]
        public string ArmLowerName
        {
            get { return armLowerName; }
            set
            {
                if (armLowerName == value) return;
                armLowerName = value;
                OnPropertyChanged("ArmLowerName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propHand)]
        [PropertyOrder(3)]
        public string HandName
        {
            get { return handName; }
            set
            {
                if (handName == value) return;
                handName = value;
                OnPropertyChanged("HandName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propFaceUpper)]
        [PropertyOrder(4)]
        public string FaceUpperName
        {
            get { return faceUpperName; }
            set
            {
                if (faceUpperName == value) return;
                faceUpperName = value;
                OnPropertyChanged("FaceUpperName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propFaceLower)]
        [PropertyOrder(5)]
        public string FaceLowerName
        {
            get { return faceLowerName; }
            set
            {
                if (faceLowerName == value) return;
                faceLowerName = value;
                OnPropertyChanged("FaceLowerName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propTorsoUpper)]
        [PropertyOrder(6)]
        public string TorsoUpperName
        {
            get { return torsoUppderName; }
            set
            {
                if (torsoUppderName == value) return;
                torsoUppderName = value;
                OnPropertyChanged("TorsoUpperName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propTorsoLower)]
        [PropertyOrder(7)]
        public string TorsoLowerName
        {
            get { return torsoLowerName; }
            set
            {
                if (torsoLowerName == value) return;
                torsoLowerName = value;
                OnPropertyChanged("TorsoLowerName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propLegUpper)]
        [PropertyOrder(8)]
        public string LegUpperName
        {
            get { return legUpperName; }
            set
            {
                if (legUpperName == value) return;
                legUpperName = value;
                OnPropertyChanged("LegUpperName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propLegLower)]
        [PropertyOrder(9)]
        public string LegLowerName
        {
            get { return legLowerName; }
            set
            {
                if (legLowerName == value) return;
                legLowerName = value;
                OnPropertyChanged("LegLowerName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propFoot)]
        [PropertyOrder(10)]
        public string FootName
        {
            get { return footName; }
            set
            {
                if (footName == value) return;
                footName = value;
                OnPropertyChanged("FootName");
            }
        }

        #endregion

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion
    }

    public class ComboTexturePropertyEnglish : INotifyPropertyChanged
    {
        #region display name
        public const string propBase = "Body";
        public const string propArmUpper = "Arm Upper";
        public const string propArmLower = "Arm Lower";
        public const string propHand = "Hand";
        public const string propFaceUpper = "Face Upper";
        public const string propFaceLower = "Face Lower";
        public const string propTorsoUpper = "Torso Upper";
        public const string propTorsoLower = "Torso Lower";
        public const string propLegUpper = "Leg Upper";
        public const string propLegLower = "Leg Lower";
        public const string propFoot = "Foot";
        public const string displayName = "Combo Textures";

        #endregion

        private string baseName;
        private string armUpperName;
        private string armLowerName;
        private string handName;
        private string faceUpperName;
        private string faceLowerName;
        private string torsoUppderName;
        private string torsoLowerName;
        private string legUpperName;
        private string legLowerName;
        private string footName;

        public void FromM2SceneNode(M2SceneNode node)
        {
            BaseName = node.GetRegionTextureFileName(ECharRegions.CR_BASE);
            ArmUpperName = node.GetRegionTextureFileName(ECharRegions.CR_ARM_UPPER);
            ArmLowerName = node.GetRegionTextureFileName(ECharRegions.CR_ARM_LOWER);
            HandName = node.GetRegionTextureFileName(ECharRegions.CR_HAND);
            FaceUpperName = node.GetRegionTextureFileName(ECharRegions.CR_FACE_UPPER);
            FaceLowerName = node.GetRegionTextureFileName(ECharRegions.CR_FACE_LOWER);
            TorsoUpperName = node.GetRegionTextureFileName(ECharRegions.CR_TORSO_UPPER);
            TorsoLowerName = node.GetRegionTextureFileName(ECharRegions.CR_TORSO_LOWER);
            LegUpperName = node.GetRegionTextureFileName(ECharRegions.CR_LEG_UPPER);
            LegLowerName = node.GetRegionTextureFileName(ECharRegions.CR_LEG_LOWER);
            FootName = node.GetRegionTextureFileName(ECharRegions.CR_FOOT);
        }

        public override string ToString()
        {
            return displayName;
        }

        #region Properties

        [ReadOnly(true)]
        [DisplayName(propBase)]
        [PropertyOrder(0)]
        public string BaseName
        {
            get { return baseName; }
            set
            {
                if (baseName == value) return;
                baseName = value;
                OnPropertyChanged("BaseName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propArmUpper)]
        [PropertyOrder(1)]
        public string ArmUpperName
        {
            get { return armUpperName; }
            set
            {
                if (armUpperName == value) return;
                armUpperName = value;
                OnPropertyChanged("ArmUpperName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propArmLower)]
        [PropertyOrder(2)]
        public string ArmLowerName
        {
            get { return armLowerName; }
            set
            {
                if (armLowerName == value) return;
                armLowerName = value;
                OnPropertyChanged("ArmLowerName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propHand)]
        [PropertyOrder(3)]
        public string HandName
        {
            get { return handName; }
            set
            {
                if (handName == value) return;
                handName = value;
                OnPropertyChanged("HandName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propFaceUpper)]
        [PropertyOrder(4)]
        public string FaceUpperName
        {
            get { return faceUpperName; }
            set
            {
                if (faceUpperName == value) return;
                faceUpperName = value;
                OnPropertyChanged("FaceUpperName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propFaceLower)]
        [PropertyOrder(5)]
        public string FaceLowerName
        {
            get { return faceLowerName; }
            set
            {
                if (faceLowerName == value) return;
                faceLowerName = value;
                OnPropertyChanged("FaceLowerName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propTorsoUpper)]
        [PropertyOrder(6)]
        public string TorsoUpperName
        {
            get { return torsoUppderName; }
            set
            {
                if (torsoUppderName == value) return;
                torsoUppderName = value;
                OnPropertyChanged("TorsoUpperName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propTorsoLower)]
        [PropertyOrder(7)]
        public string TorsoLowerName
        {
            get { return torsoLowerName; }
            set
            {
                if (torsoLowerName == value) return;
                torsoLowerName = value;
                OnPropertyChanged("TorsoLowerName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propLegUpper)]
        [PropertyOrder(8)]
        public string LegUpperName
        {
            get { return legUpperName; }
            set
            {
                if (legUpperName == value) return;
                legUpperName = value;
                OnPropertyChanged("LegUpperName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propLegLower)]
        [PropertyOrder(9)]
        public string LegLowerName
        {
            get { return legLowerName; }
            set
            {
                if (legLowerName == value) return;
                legLowerName = value;
                OnPropertyChanged("LegLowerName");
            }
        }

        [ReadOnly(true)]
        [DisplayName(propFoot)]
        [PropertyOrder(10)]
        public string FootName
        {
            get { return footName; }
            set
            {
                if (footName == value) return;
                footName = value;
                OnPropertyChanged("FootName");
            }
        }

        #endregion

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion
    }

    public class CharacterProperty : INotifyPropertyChanged
    {
        #region display name
        public const string cateMainModel = "[0] 主模型";
        public const string cateHead = "[1] 头部";
        public const string cateLeftShoulder = "[2] 左肩";
        public const string cateRightShoulder = "[3] 右肩";
        public const string cateLeftHand = "[4] 左手武器";
        public const string cateRightHand = "[5] 右手武器";

        public const string propM2File = "M2名";
        public const string propM2LongFile = "M2文件名";
        public const string propM2Textures = "M2纹理";
        public const string propBodyTexture = "身体纹理";
        public const string propCapeTexture = "披风纹理";
        public const string propHairTexture = "头发纹理";
        public const string propFurTexture = "毛发纹理";
        public const string propItemTexture = "物品纹理";

        #endregion
    
        private string m2FileName;
        private string m2LongFileName;
        private string[] m2TextureFileNames;
        private ComboTextureProperty comboTextureNames = new ComboTextureProperty();
        private string texCapeFileName;
        private string texHairFileName;
        private string texFurFileName;

        private string headM2FileName;
        private string headM2LongFileName;
        private string headTexItemFileName;

        private string leftShoulderM2FileName;
        private string leftShoulderM2LongFileName;
        private string leftShoulderTexItemFileName;

        private string rightShoulderM2FileName;
        private string rightShoulderM2LongFileName;
        private string rightShoulderTexItemFileName;

        private string leftHandM2FileName;
        private string leftHandM2LongFileName;
        private string leftHandTexItemFileName;

        private string rightHandM2FileName;
        private string rightHandM2LongFileName;
        private string rightHandTexItemFileName;

        public void FromM2SceneNode(M2SceneNode node)
        {
            SFileM2 filem2 = node.FileM2;
            M2FileName = filem2.name;
            M2LongFileName = filem2.longname;
            M2TextureNames = node.TextureNames;
            comboTextureNames.FromM2SceneNode(node);
            TexCapeFileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
            TexHairFileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_HAIR);
            TexFurFileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_FUR);

            //head
            {
                M2SceneNode head = node.HeadSceneNode;
                if (head != null)
                {
                    filem2 = head.FileM2;
                    HeadM2FileName = filem2.name;
                    HeadM2LongFileName = filem2.longname;
                    HeadTexItemFileName = head.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
                }
                else
                {
                    HeadM2FileName = "";
                    HeadM2LongFileName = "";
                    HeadTexItemFileName = "";
                }
            }
           
            //left shoulder
            {
                M2SceneNode leftShoulder = node.LeftShoulderSceneNode;
                if (leftShoulder != null)
                {
                    filem2 = leftShoulder.FileM2;
                    LeftShoulderM2FileName = filem2.name;
                    LeftShoulderM2LongFileName = filem2.longname;
                    LeftShoulderTexItemFileName = leftShoulder.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
                }
                else
                {
                    LeftShoulderM2FileName = "";
                    LeftShoulderM2LongFileName = "";
                    LeftShoulderTexItemFileName = "";
                }
            }
            
            //right shoulder
            {
                M2SceneNode rightShoulder = node.RightShoulderSceneNode;
                if (rightShoulder != null)
                {
                    filem2 = rightShoulder.FileM2;
                    RightShoulderM2FileName = filem2.name;
                    RightShoulderM2LongFileName = filem2.longname;
                    RightShoulderTexItemFileName = rightShoulder.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
                }
                else
                {
                    RightShoulderM2FileName = "";
                    RightShoulderM2LongFileName = "";
                    RightShoulderTexItemFileName = "";
                }
            }
          
            //left hand
            {
                M2SceneNode leftHand = node.LeftHandSceneNode;
                if (leftHand != null)
                {
                    filem2 = leftHand.FileM2;
                    LeftHandM2FileName = filem2.name;
                    LeftHandM2LongFileName = filem2.longname;
                    LeftHandTexItemFileName = leftHand.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
                }
                else
                {
                    LeftHandM2FileName = "";
                    LeftHandM2LongFileName = "";
                    LeftHandTexItemFileName = "";
                }
            }
           
            //right hand
            {
                M2SceneNode rightHand = node.RightHandSceneNode;
                if (rightHand != null)
                {
                    filem2 = rightHand.FileM2;
                    RightHandM2FileName = filem2.name;
                    RightHandM2LongFileName = filem2.longname;
                    RightHandTexItemFileName = rightHand.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
                }
                else
                {
                    RightHandM2FileName = "";
                    RightHandM2LongFileName = "";
                    RightHandTexItemFileName = "";
                }
            }
           
        }

        #region MainModel Properties

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        [PropertyOrder(0)]
        public string M2FileName
        {
            get { return m2FileName; }
            set
            {
                if (m2FileName == value) return;
                m2FileName = value;
                OnPropertyChanged("M2FileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        [PropertyOrder(1)]
        public string M2LongFileName
        {
            get { return m2LongFileName; }
            set
            {
                if (m2LongFileName == value) return;
                m2LongFileName = value;
                OnPropertyChanged("M2LongFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2Textures)]
        [PropertyOrder(2)]
        public string[] M2TextureNames
        {
            get { return m2TextureFileNames; }
            set
            {
                if (m2TextureFileNames == value) return;
                m2TextureFileNames = value;
                OnPropertyChanged("M2TextureFileNames");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [TypeConverter(typeof(ExpandableObjectConverter))]
        [PropertyOrder(3)]
        [DisplayName(propBodyTexture)]
        public ComboTextureProperty ComboTextureNames
        {
            get { return comboTextureNames; }
            set
            {
                if (comboTextureNames == value) return;
                comboTextureNames = value;
                OnPropertyChanged("ComboTextureNames");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propCapeTexture)]
        [PropertyOrder(4)]
        public string TexCapeFileName
        {
            get { return texCapeFileName; }
            set
            {
                if (texCapeFileName == value) return;
                texCapeFileName = value;
                OnPropertyChanged("TexCapeFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propHairTexture)]
        [PropertyOrder(5)]
        public string TexHairFileName
        {
            get { return texHairFileName; }
            set
            {
                if (texHairFileName == value) return;
                texHairFileName = value;
                OnPropertyChanged("TexHairFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propFurTexture)]
        [PropertyOrder(6)]
        public string TexFurFileName
        {
            get { return texFurFileName; }
            set
            {
                if (texFurFileName == value) return;
                texFurFileName = value;
                OnPropertyChanged("TexFurFileName");
            }
        }

        #endregion

        #region Head Properties

        [Category(cateHead)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string HeadM2FileName
        {
            get { return headM2FileName; }
            set
            {
                if (headM2FileName == value) return;
                headM2FileName = value;
                OnPropertyChanged("HeadM2FileName");
            }
        }

        [Category(cateHead)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string HeadM2LongFileName
        {
            get { return headM2LongFileName; }
            set
            {
                if (headM2LongFileName == value) return;
                headM2LongFileName = value;
                OnPropertyChanged("HeadM2LongFileName");
            }
        }

        [Category(cateHead)]
        [ReadOnly(true)]
        [DisplayName(propItemTexture)]
        public string HeadTexItemFileName
        {
            get { return headTexItemFileName; }
            set
            {
                if (headTexItemFileName == value) return;
                headTexItemFileName = value;
                OnPropertyChanged("HeadTexItemFileName");
            }
        }

        #endregion

        #region Left Shoulder Properties

        [Category(cateLeftShoulder)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string LeftShoulderM2FileName
        {
            get { return leftShoulderM2FileName; }
            set
            {
                if (leftShoulderM2FileName == value) return;
                leftShoulderM2FileName = value;
                OnPropertyChanged("LeftShoulderM2FileName");
            }
        }

        [Category(cateLeftShoulder)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string LeftShoulderM2LongFileName
        {
            get { return leftShoulderM2LongFileName; }
            set
            {
                if (leftShoulderM2LongFileName == value) return;
                leftShoulderM2LongFileName = value;
                OnPropertyChanged("LeftShoulderM2LongFileName");
            }
        }

        [Category(cateLeftShoulder)]
        [ReadOnly(true)]
        [DisplayName(propItemTexture)]
        public string LeftShoulderTexItemFileName
        {
            get { return leftShoulderTexItemFileName; }
            set
            {
                if (leftShoulderTexItemFileName == value) return;
                leftShoulderTexItemFileName = value;
                OnPropertyChanged("LeftShoulderTexItemFileName");
            }
        }

        #endregion

        #region Right Shoulder Properties

        [Category(cateRightShoulder)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string RightShoulderM2FileName
        {
            get { return rightShoulderM2FileName; }
            set
            {
                if (rightShoulderM2FileName == value) return;
                rightShoulderM2FileName = value;
                OnPropertyChanged("RightShoulderM2FileName");
            }
        }

        [Category(cateRightShoulder)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string RightShoulderM2LongFileName
        {
            get { return rightShoulderM2LongFileName; }
            set
            {
                if (rightShoulderM2LongFileName == value) return;
                rightShoulderM2LongFileName = value;
                OnPropertyChanged("RightShoulderM2LongFileName");
            }
        }

        [Category(cateRightShoulder)]
        [ReadOnly(true)]
        [DisplayName(propItemTexture)]
        public string RightShoulderTexItemFileName
        {
            get { return rightShoulderTexItemFileName; }
            set
            {
                if (rightShoulderTexItemFileName == value) return;
                rightShoulderTexItemFileName = value;
                OnPropertyChanged("RightShoulderTexItemFileName");
            }
        }

        #endregion

        #region Left Hand Properties

        [Category(cateLeftHand)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string LeftHandM2FileName
        {
            get { return leftHandM2FileName; }
            set
            {
                if (leftHandM2FileName == value) return;
                leftHandM2FileName = value;
                OnPropertyChanged("LeftHandM2FileName");
            }
        }

        [Category(cateLeftHand)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string LeftHandM2LongFileName
        {
            get { return leftHandM2LongFileName; }
            set
            {
                if (leftHandM2LongFileName == value) return;
                leftHandM2LongFileName = value;
                OnPropertyChanged("LeftHandM2LongFileName");
            }
        }

        [Category(cateLeftHand)]
        [ReadOnly(true)]
        [DisplayName(propItemTexture)]
        public string LeftHandTexItemFileName
        {
            get { return leftHandTexItemFileName; }
            set
            {
                if (leftHandTexItemFileName == value) return;
                leftHandTexItemFileName = value;
                OnPropertyChanged("LeftHandTexItemFileName");
            }
        }

        #endregion

        #region Right Hand Properties

        [Category(cateRightHand)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string RightHandM2FileName
        {
            get { return rightHandM2FileName; }
            set
            {
                if (rightHandM2FileName == value) return;
                rightHandM2FileName = value;
                OnPropertyChanged("RightHandM2FileName");
            }
        }

        [Category(cateRightHand)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string RightHandM2LongFileName
        {
            get { return rightHandM2LongFileName; }
            set
            {
                if (rightHandM2LongFileName == value) return;
                rightHandM2LongFileName = value;
                OnPropertyChanged("RightHandM2LongFileName");
            }
        }

        [Category(cateRightHand)]
        [ReadOnly(true)]
        [DisplayName(propItemTexture)]
        public string RightHandTexItemFileName
        {
            get { return rightHandTexItemFileName; }
            set
            {
                if (rightHandTexItemFileName == value) return;
                rightHandTexItemFileName = value;
                OnPropertyChanged("RightHandTexItemFileName");
            }
        }

        #endregion

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion
    }

    public class CharacterPropertyEnglish : INotifyPropertyChanged
    {
        #region display name
        public const string cateMainModel = "[0] Main Model";
        public const string cateHead = "[1] Head";
        public const string cateLeftShoulder = "[2] Left Shoulder";
        public const string cateRightShoulder = "[3] Right Shoulder";
        public const string cateLeftHand = "[4] Left Hand";
        public const string cateRightHand = "[5] Right Hand";

        public const string propM2File = "M2 Name";
        public const string propM2LongFile = "M2 FileName";
        public const string propM2Textures = "M2 Textures";
        public const string propBodyTexture = "Body Texture";
        public const string propCapeTexture = "Cape Texture";
        public const string propHairTexture = "Hair Texture";
        public const string propFurTexture = "Fur Texture";
        public const string propItemTexture = "Item Texture";

        #endregion

        private string m2FileName;
        private string m2LongFileName;
        private string[] m2TextureFileNames;
        private ComboTexturePropertyEnglish comboTextureNames = new ComboTexturePropertyEnglish();
        private string texCapeFileName;
        private string texHairFileName;
        private string texFurFileName;

        private string headM2FileName;
        private string headM2LongFileName;
        private string headTexItemFileName;

        private string leftShoulderM2FileName;
        private string leftShoulderM2LongFileName;
        private string leftShoulderTexItemFileName;

        private string rightShoulderM2FileName;
        private string rightShoulderM2LongFileName;
        private string rightShoulderTexItemFileName;

        private string leftHandM2FileName;
        private string leftHandM2LongFileName;
        private string leftHandTexItemFileName;

        private string rightHandM2FileName;
        private string rightHandM2LongFileName;
        private string rightHandTexItemFileName;

        public void FromM2SceneNode(M2SceneNode node)
        {
            SFileM2 filem2 = node.FileM2;
            M2FileName = filem2.name;
            M2LongFileName = filem2.longname;
            M2TextureNames = node.TextureNames;
            comboTextureNames.FromM2SceneNode(node);
            TexCapeFileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
            TexHairFileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_HAIR);
            TexFurFileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_FUR);

            //head
            {
                M2SceneNode head = node.HeadSceneNode;
                if (head != null)
                {
                    filem2 = head.FileM2;
                    HeadM2FileName = filem2.name;
                    HeadM2LongFileName = filem2.longname;
                    HeadTexItemFileName = head.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
                }
                else
                {
                    HeadM2FileName = "";
                    HeadM2LongFileName = "";
                    HeadTexItemFileName = "";
                }
            }

            //left shoulder
            {
                M2SceneNode leftShoulder = node.LeftShoulderSceneNode;
                if (leftShoulder != null)
                {
                    filem2 = leftShoulder.FileM2;
                    LeftShoulderM2FileName = filem2.name;
                    LeftShoulderM2LongFileName = filem2.longname;
                    LeftShoulderTexItemFileName = leftShoulder.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
                }
                else
                {
                    LeftShoulderM2FileName = "";
                    LeftShoulderM2LongFileName = "";
                    LeftShoulderTexItemFileName = "";
                }
            }

            //right shoulder
            {
                M2SceneNode rightShoulder = node.RightShoulderSceneNode;
                if (rightShoulder != null)
                {
                    filem2 = rightShoulder.FileM2;
                    RightShoulderM2FileName = filem2.name;
                    RightShoulderM2LongFileName = filem2.longname;
                    RightShoulderTexItemFileName = rightShoulder.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
                }
                else
                {
                    RightShoulderM2FileName = "";
                    RightShoulderM2LongFileName = "";
                    RightShoulderTexItemFileName = "";
                }
            }

            //left hand
            {
                M2SceneNode leftHand = node.LeftHandSceneNode;
                if (leftHand != null)
                {
                    filem2 = leftHand.FileM2;
                    LeftHandM2FileName = filem2.name;
                    LeftHandM2LongFileName = filem2.longname;
                    LeftHandTexItemFileName = leftHand.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
                }
                else
                {
                    LeftHandM2FileName = "";
                    LeftHandM2LongFileName = "";
                    LeftHandTexItemFileName = "";
                }
            }

            //right hand
            {
                M2SceneNode rightHand = node.RightHandSceneNode;
                if (rightHand != null)
                {
                    filem2 = rightHand.FileM2;
                    RightHandM2FileName = filem2.name;
                    RightHandM2LongFileName = filem2.longname;
                    RightHandTexItemFileName = rightHand.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_CAPE);
                }
                else
                {
                    RightHandM2FileName = "";
                    RightHandM2LongFileName = "";
                    RightHandTexItemFileName = "";
                }
            }

        }

        #region MainModel Properties

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        [PropertyOrder(0)]
        public string M2FileName
        {
            get { return m2FileName; }
            set
            {
                if (m2FileName == value) return;
                m2FileName = value;
                OnPropertyChanged("M2FileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        [PropertyOrder(1)]
        public string M2LongFileName
        {
            get { return m2LongFileName; }
            set
            {
                if (m2LongFileName == value) return;
                m2LongFileName = value;
                OnPropertyChanged("M2LongFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2Textures)]
        [PropertyOrder(2)]
        public string[] M2TextureNames
        {
            get { return m2TextureFileNames; }
            set
            {
                if (m2TextureFileNames == value) return;
                m2TextureFileNames = value;
                OnPropertyChanged("M2TextureFileNames");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [TypeConverter(typeof(ExpandableObjectConverter))]
        [PropertyOrder(3)]
        [DisplayName(propBodyTexture)]
        public ComboTexturePropertyEnglish ComboTextureNames
        {
            get { return comboTextureNames; }
            set
            {
                if (comboTextureNames == value) return;
                comboTextureNames = value;
                OnPropertyChanged("ComboTextureNames");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propCapeTexture)]
        [PropertyOrder(4)]
        public string TexCapeFileName
        {
            get { return texCapeFileName; }
            set
            {
                if (texCapeFileName == value) return;
                texCapeFileName = value;
                OnPropertyChanged("TexCapeFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propHairTexture)]
        [PropertyOrder(5)]
        public string TexHairFileName
        {
            get { return texHairFileName; }
            set
            {
                if (texHairFileName == value) return;
                texHairFileName = value;
                OnPropertyChanged("TexHairFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propFurTexture)]
        [PropertyOrder(6)]
        public string TexFurFileName
        {
            get { return texFurFileName; }
            set
            {
                if (texFurFileName == value) return;
                texFurFileName = value;
                OnPropertyChanged("TexFurFileName");
            }
        }

        #endregion

        #region Head Properties

        [Category(cateHead)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string HeadM2FileName
        {
            get { return headM2FileName; }
            set
            {
                if (headM2FileName == value) return;
                headM2FileName = value;
                OnPropertyChanged("HeadM2FileName");
            }
        }

        [Category(cateHead)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string HeadM2LongFileName
        {
            get { return headM2LongFileName; }
            set
            {
                if (headM2LongFileName == value) return;
                headM2LongFileName = value;
                OnPropertyChanged("HeadM2LongFileName");
            }
        }

        [Category(cateHead)]
        [ReadOnly(true)]
        [DisplayName(propItemTexture)]
        public string HeadTexItemFileName
        {
            get { return headTexItemFileName; }
            set
            {
                if (headTexItemFileName == value) return;
                headTexItemFileName = value;
                OnPropertyChanged("HeadTexItemFileName");
            }
        }

        #endregion

        #region Left Shoulder Properties

        [Category(cateLeftShoulder)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string LeftShoulderM2FileName
        {
            get { return leftShoulderM2FileName; }
            set
            {
                if (leftShoulderM2FileName == value) return;
                leftShoulderM2FileName = value;
                OnPropertyChanged("LeftShoulderM2FileName");
            }
        }

        [Category(cateLeftShoulder)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string LeftShoulderM2LongFileName
        {
            get { return leftShoulderM2LongFileName; }
            set
            {
                if (leftShoulderM2LongFileName == value) return;
                leftShoulderM2LongFileName = value;
                OnPropertyChanged("LeftShoulderM2LongFileName");
            }
        }

        [Category(cateLeftShoulder)]
        [ReadOnly(true)]
        [DisplayName(propItemTexture)]
        public string LeftShoulderTexItemFileName
        {
            get { return leftShoulderTexItemFileName; }
            set
            {
                if (leftShoulderTexItemFileName == value) return;
                leftShoulderTexItemFileName = value;
                OnPropertyChanged("LeftShoulderTexItemFileName");
            }
        }

        #endregion

        #region Right Shoulder Properties

        [Category(cateRightShoulder)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string RightShoulderM2FileName
        {
            get { return rightShoulderM2FileName; }
            set
            {
                if (rightShoulderM2FileName == value) return;
                rightShoulderM2FileName = value;
                OnPropertyChanged("RightShoulderM2FileName");
            }
        }

        [Category(cateRightShoulder)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string RightShoulderM2LongFileName
        {
            get { return rightShoulderM2LongFileName; }
            set
            {
                if (rightShoulderM2LongFileName == value) return;
                rightShoulderM2LongFileName = value;
                OnPropertyChanged("RightShoulderM2LongFileName");
            }
        }

        [Category(cateRightShoulder)]
        [ReadOnly(true)]
        [DisplayName(propItemTexture)]
        public string RightShoulderTexItemFileName
        {
            get { return rightShoulderTexItemFileName; }
            set
            {
                if (rightShoulderTexItemFileName == value) return;
                rightShoulderTexItemFileName = value;
                OnPropertyChanged("RightShoulderTexItemFileName");
            }
        }

        #endregion

        #region Left Hand Properties

        [Category(cateLeftHand)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string LeftHandM2FileName
        {
            get { return leftHandM2FileName; }
            set
            {
                if (leftHandM2FileName == value) return;
                leftHandM2FileName = value;
                OnPropertyChanged("LeftHandM2FileName");
            }
        }

        [Category(cateLeftHand)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string LeftHandM2LongFileName
        {
            get { return leftHandM2LongFileName; }
            set
            {
                if (leftHandM2LongFileName == value) return;
                leftHandM2LongFileName = value;
                OnPropertyChanged("LeftHandM2LongFileName");
            }
        }

        [Category(cateLeftHand)]
        [ReadOnly(true)]
        [DisplayName(propItemTexture)]
        public string LeftHandTexItemFileName
        {
            get { return leftHandTexItemFileName; }
            set
            {
                if (leftHandTexItemFileName == value) return;
                leftHandTexItemFileName = value;
                OnPropertyChanged("LeftHandTexItemFileName");
            }
        }

        #endregion

        #region Right Hand Properties

        [Category(cateRightHand)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string RightHandM2FileName
        {
            get { return rightHandM2FileName; }
            set
            {
                if (rightHandM2FileName == value) return;
                rightHandM2FileName = value;
                OnPropertyChanged("RightHandM2FileName");
            }
        }

        [Category(cateRightHand)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string RightHandM2LongFileName
        {
            get { return rightHandM2LongFileName; }
            set
            {
                if (rightHandM2LongFileName == value) return;
                rightHandM2LongFileName = value;
                OnPropertyChanged("RightHandM2LongFileName");
            }
        }

        [Category(cateRightHand)]
        [ReadOnly(true)]
        [DisplayName(propItemTexture)]
        public string RightHandTexItemFileName
        {
            get { return rightHandTexItemFileName; }
            set
            {
                if (rightHandTexItemFileName == value) return;
                rightHandTexItemFileName = value;
                OnPropertyChanged("RightHandTexItemFileName");
            }
        }

        #endregion

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion
    }
}
