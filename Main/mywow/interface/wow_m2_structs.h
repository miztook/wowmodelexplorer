#pragma once

#include "base.h"

namespace M2
{

	//wow模型定义
#	pragma pack (1)

	//------------------------------------------------------------------------------
	// WotLK version
	struct Header20 { 				      
		/*0x000*/   u8      _magic[4]; 	             // "MD20"																					  			
		/*0x004*/   u8   _version[4];		     // 0x80 10 00 00 (first digit of the build the format was last updated)												  	  			
		/*0x008*/   u32  _modelNameLength;		     // Length of the model's name																	  	  			
		/*0x00C*/   u32  _modelNameOffset; 	     // Offset to the name																			  			   
		/*0x010*/   u32  _GlobalModelFlags;	     // 1: tilt x, 2: tilt y, 4:, 8: add another field in header, 16: ; (no other flags as of 3.1.1); list at M2/WotLK/flags --schlumpf_ 02:21, 23 April 2009 (CEST)	  	  																							       

		/*0x014*/   u32  _nGlobalSequences;	     // A list of timestamps.														        						     
		/*0x018*/   u32  _ofsGlobalSequences;	     // 																        						     

		/*0x01C*/   u32  _nAnimations;		     // Information about the animations in the model.  										        						     
		/*0x020*/   u32  _ofsAnimations;		     // 																        						     

		/*0x024*/   u32  _nAnimationLookup;	     // Mapping of global IDs to the entries in the Animation sequences block.  							        						     
		/*0x028*/   u32  _ofsAnimationLookup;	     // 																        						     

		/*0x02C*/   u32  _nBones;  		     // Information about the bones in this model.											        						     
		/*0x030*/   u32  _ofsBones;		     // 																        						     

		/*0x034*/   u32  _nKeyBoneLookup;  	     // Lookup table for key skeletal bones.												        						     
		/*0x038*/   u32  _ofsKeyBoneLookup;	     // 																        						     

		/*0x03C*/   u32  _nVertices;		     // Vertices of the model.  													        						     
		/*0x040*/   u32  _ofsVertices;		     // Views (LOD) are now in .skins.  												        						     
		/*0x044*/   u32  _nViews;  		     // Color definitions.														        						     

		/*0x048*/   u32  _nColors; 		     // # of colors in table
		/*0x04C*/   u32  _ofsColors;		     // 													        						     

		/*0x050*/   u32  _nTextures;		     // # of transparency values in table																        						     
		/*0x054*/   u32  _ofsTextures;		     // 													        						     

		/*0x058*/   u32  _nTransparency;		     // 																        						     
		/*0x05C*/   u32  _ofsTransparency; 	     // 																        						     

		/*0x060*/   u32  _nTextureanimations;	     // 																        						     
		/*0x064*/   u32  _ofsTextureanimations;	     // 																        						     

		/*0x068*/   u32  _nTexReplace;		     // Replaceable Textures.														        						     
		/*0x06C*/   u32  _ofsTexReplace;		     // 																        						     

		/*0x070*/   u32  _nRenderFlags;		     // Blending modes / render flags.  												        						     
		/*0x074*/   u32  _ofsRenderFlags;  	     // 																        						     

		/*0x078*/   u32  _nBoneLookupTable;	     // A bone lookup table.														        						     
		/*0x07C*/   u32  _ofsBoneLookupTable;	     // 																        						     

		/*0x080*/   u32  _nTexLookup;		     // The same for textures.  													        						     
		/*0x084*/   u32  _ofsTexLookup;		     // 																        						     

		/*0x088*/   u32  _nTexUnits;		     // And texture units. Somewhere they have to be too.										        						     
		/*0x08C*/   u32  _ofsTexUnits;		     // 																        						     

		/*0x090*/   u32  _nTransLookup;		     // Everything needs its lookup. Here are the transparencies.
		/*0x094*/   u32  _ofsTransLookup;  	     // 																        						     

		/*0x098*/   u32  _nTexAnimLookup;  	     // Wait. Do we have animated Textures? Wasn't ofsTexAnims deleted? oO		
		/*0x09C*/   u32  _ofsTexAnimLookup;	     // Noone knows. Meeh, they are here.												        						     

		/*0x0A0*/   aabbox3df      _boundingbox;		     //bounding 																        						     
		/*0x0B8*/   f32     _boundingRadius;
		/*0x0BC*/   aabbox3df      _vertexBox;                  //collision
		/*0x0D4*/   f32     _vertexRadius; 

		/*0x0D8*/   u32  _nBoundingTriangles;	     // Our bounding volumes. Similar structure like in the old ofsViews.	
		/*0x0DC*/   u32  _ofsBoundingTriangles;	     // 																        						     

		/*0x0E0*/   u32  _nBoundingVertices;	     // 																        						     
		/*0x0E4*/   u32  _ofsBoundingVertices;	     // 																        						     

		/*0x0E8*/   u32  _nBoundingNormals;	     // 																        						     
		/*0x0EC*/   u32  _ofsBoundingNormals;	     // 																        						     

		/*0x0F0*/   u32  _nAttachments;		     // Attachments are for weapons etc.												        						     
		/*0x0F4*/   u32  _ofsAttachments;  	     // 																        						     

		/*0x0F8*/   u32  _nAttachLookup;		     // Of course with a lookup.													        						     
		/*0x0FC*/   u32  _ofsAttachLookup; 	     // 																        						     

		/*0x100*/   u32  _nEvents;  	     //												        						     
		/*0x104*/   u32  _ofsEvents;	     // Used for playing sounds when dying and a lot else.																        						     
		/*0x108*/   u32  _nLights; 		     // Lights are mainly used in loginscreens but in wands and some doodads too.	
		/*0x10C*/   u32  _ofsLights;		     // 																        						     

		/*0x110*/   u32  _nCameras;		     // The cameras are present in most models for having a model in the Character-Tab. 		
		/*0x114*/   u32  _ofsCameras;		     // 																        						     

		/*0x118*/   u32  _nCameraLookup;		     // And lookup-time again.  													        						     
		/*0x11C*/   u32  _ofsCameraLookup; 	     // 																        						     

		/*0x120*/   u32  _nRibbonEmitters; 	     // Things swirling around. See the CoT-entrance for light-trails.  			
		/*0x124*/   u32  _ofsRibbonEmitters;	     // 																        						     

		/*0x128*/   u32  _nParticleEmitters;	     // Spells and weapons, doodads and loginscreens use them. Blood dripping of a blade? Particles.	
		/*0x12C*/   u32  _ofsParticleEmitters;	     	        						     
	};

	struct Header21
	{
		u8      _magic[4]; 	             // "MD20"
		u32 unknown;
		Header20	_header20;
	};

	//------------------------------------------------------------------------------
	struct vertex {
		/*0x00*/    vector3df     _Position;       	 	     // A vector to the position of the vertex.
		/*0x0C*/    u8   _BoneWeight[4];     	     // The vertex weight for 4 bones.
		/*0x10*/    u8   _BoneIndices[4];    	     // Which are referenced here.
		/*0x14*/    vector3df     _Normal;         		     // A normal vector.
		/*0x20*/    vector2df     _TextureCoords0;	  	     // Coordinates for a texture.
		/*0x28*/    vector2df     _TextureCoords1;        	     // Null?	
	};

	//------------------------------------------------------------------------------
	struct animseq {
		/*0x00*/    u16  _AnimationID;     	     // Animation id in AnimationData.dbc										      
		/*0x02*/    u16  _SubAnimationID;  	     // Sub-animation id: Which number in a row of animations this one is.						      
		/*0x04*/    u32  _Length;          	     // The length (timestamps) of the animation. I believe this actually the length of the animation in milliseconds.        
		/*0x08*/    float     _MovingSpeed;     	     // As 2.x says: moving speed for walk/run animations.								      
		/*0x0C*/    u32  _Flags;           	     // One thing I saw in the source is that "-1 animationblocks" in bones wont get parsed if 0x20 is not set. 	      
		/*0x10*/    u32  _Flags_2;         	     // Only the first 4 bits are the actual flags. The rest is 1. Seen flags: 0,3,6,7  				      
		/*0x14*/    u32  _Unknown_1;       	     // These two are connected. Most of the time, they are 0.  							      
		/*0x18*/    u32  _Unknown_2;       	     // But if there is data in one, there is data in both of them.							      
		/*0x1C*/    u32  _PlaybackSpeed;   	     // Values: 0, 50, 100, 150, 200, 250, 300, 350, 500.								      
		/*0x20*/    aabbox3df      _BBox;  	             	     // A Bounding Box made out of 2 vectors.										      
		/*0x38*/    float     _Radius;                       // The radius.													      
		/*0x3C*/    s16   _NextAnimation;                // Id of the following animation of this AnimationID, points to an Index or is -1 if none. 			      
		/*0x3E*/    s16  _Index;                        // Id in the list of animations.	
	};

	//------------------------------------------------------------------------------
	struct sequence {
		/*0x00*/    u32  _NValues;
		/*0x04*/    u32  _SequencesOfs;
	};

	//------------------------------------------------------------------------------
	struct animblock {
		/*0x00*/    s16   _Interpolation;         	     // interpolation type
		/*0x02*/    s16   _SequenceID;	      	     // global sequence ID or -1
		// 	/*0x04*/    u32  _nRanges;	             // number of (int, int) interpolation ranges	 
		// 	/*0x08*/    u32  _rangesOfs;                  // offset to interpolation ranges		    	 
		/*0x0C*/    u32  _Ntimings;                     // number of (int) timestamps
		/*0x10*/    u32  _TimingsOfs;                   // offset to timestamps
		/*0x14*/    u32  _Nvalues;                      // number of values
		/*0x18*/    u32  _ValuesOfs;                    // offset to values
	};

	//------------------------------------------------------------------------------
	struct bone {
		/*0x00*/    s32   _AnimationSeq;    	     // Index into Animation sequences or -1.					      
		/*0x04*/    u32  _Flags;	        	     // Only known flags: 8 - billborded and 512 - transformed  				    
		/*0x08*/    s16   _ParentBone;      	     // Parent bone ID or -1 if there is none.  				      
		/*0x0A*/    s16  _GeosetID;        	     // A geoset for this bone. 						      
		/*0x0C*/    u32  _Unknown;         	     // 									      
		/*0x10*/    animblock _Translation;     	     // An animationblock for translation. Should be 3*floats.  		      
		/*0x24*/    animblock _Rotation;        	     // An animationblock for rotation. Should be 4*shorts, see Quaternion values and 2.x.    
		/*0x38*/    animblock _Scaling;         	     // An animationblock for scaling. Should be 3*floats.					    
		/*0x4C*/    vector3df     _PivotPoint;      	     // The pivot point of that bone. Its a vector.		
	};

	//------------------------------------------------------------------------------
	// Attachments 
	struct attach {
		/*0x00*/    u32  _Id;	   		     // Just an id. Is referenced in the lookup-block below too.
		/*0x04*/    s32  _Bone;	   		     // Bone it is attached to
		/*0x08*/    vector3df     _Position;   		     // Position (relative to the bone)
		/*0x14*/    animblock _Data;	  		     // Its an integer in the data. It has been 1 on all models I saw. Whatever.
	};

	//------------------------------------------------------------------------------
	struct texture {
		/*0x000*/   u32  _type;	    		     // The type of the texture : 0 for regular textures
		/*0x004*/   u32  _flags;     		     // Textures have some flags. 
		/*0x008*/   u32  _lenFilename; 		     // Here is the length of the filename, if the type is not "0 - hardcoded" then it's just 1 and points to a zero
		/*0x00C*/   u32  _ofsFilename; 		     // And the offset to the filename.
	};

	//------------------------------------------------------------------------------
	struct light {
		/*0x00*/    u16  _type;			     // Types are listed below. 					     
		/*0x02*/    s16   _bone;			     // If its attached to a bone, this is the bone. Else here is a nice -1. 
		/*0x04*/    vector3df     _pos;     	      	     // Where is this light?						     
		/*0x10*/    animblock _ambientColor; 		     // The ambient color. Three floats for RGB.			     
		/*0x24*/    animblock _ambientIntensity;	     // A float for the intensity.		     	 	      
		/*0x38*/    animblock _diffuseColor;		     // The diffuse color. Three floats for RGB.     	 	      
		/*0x4C*/    animblock _diffuseIntensity;	     // A float for the intensity again.	     	 	      
		/*0x60*/    animblock _attenuationStart;	     // This defines, where the light starts to be.  	 	      
		/*0x74*/    animblock _attenuationEnd;  	     // And where it stops.		   					  
		/*0x88*/    animblock _unknown; 		     // Its an integer and usually 1.	  
	};

	//------------------------------------------------------------------------------
	struct skin_header {
		/*0x00*/    char      _magic[4];	// "SKIN"

		/*0x04*/    u32  _nIndices;        // 	  
		/*0x08*/    u32  _ofsIndices;      // Indices used in this View.

		/*0x0C*/    u32  _nTriangles;      // 	  
		/*0x10*/    u32  _ofsTriangles;    // The triangles made with them.

		/*0x14*/    u32  _nProperties;     // 	  
		/*0x18*/    u32  _ofsProperties;   // Properties of the vertices.

		/*0x1C*/    u32  _nSubmeshes;      // 	  
		/*0x20*/    u32  _ofsSubmeshes;    // Submeshes (Geosets) of this View.

		/*0x24*/    u32  _nTextureUnits;   // 	  
		/*0x28*/    u32  _ofsTextureUnits; // Texture Units.
		/*0x2C*/    u32  _LOD;             // LOD distance or something else
	};

	//------------------------------------------------------------------------------
	struct submesh {
		/*0x00*/    u32  _ID;	        // Mesh part ID, see below.
		/*0x04*/    u16  _startVertex;     // Starting vertex number.
		/*0x06*/    u16  _nVertices;       // Number of vertices.
		/*0x08*/    u16  _startTriangle;   // Starting triangle index (that's 3* the number of triangles drawn so far).
		/*0x0A*/    u16  _nTriangles;      // Number of triangle indices.
		/*0x0C*/    u16  _nBones;	        // Number of elements in the bone lookup table.
		/*0x0E*/    u16  _startBones;      // Starting index in the bone lookup table.
		/*0x10*/    u16  _unknown;         // 
		/*0x12*/    u16  _rootBone;        // Not sure.
		/*0x14*/    vector3df   _minBox;     	// Some Vector. Not sure about it being the Position.
						  vector3df	_maxBox;
		/*0x20*/    float     _radius;       // New Floats since BC. 	 
	};

	//------------------------------------------------------------------------------
	struct textureUnit {
		/*0x00*/  u16    _flags;                        // Flags
		/*0x02*/  u16     _shading;                     // If set to 0x8000: shaders. Used in skyboxes to ditch the need for depth buffering.
		/*0x04*/  s16    _submeshIdx;                   // Submesh index
		/*0x06*/  s16    _submeshIdx2;  		     // Submesh index (repeated?)
		/*0x08*/  s16     _colorIdx;                     // Color index or -1
		/*0x0A*/  s16    _renderFlagsIdx;		     // Index into render flags table
		/*0x0C*/  s16    _textureUnit;		     // Texture unit number (0 or 1 - index into the texture unit lookup table)
		/*0x0E*/  u16    _mode;		             // Its actually two uint8s defining the shader used. Everything below this is in binary. X represents a variable digit
		/*0x10*/  s16    _textureIdx;		     // Texture to use (index into the texture lookup table)
		/*0x12*/  s16    _textureUnit2;		     // Texture unit number (repeated?)
		/*0x14*/  s16    _transparencyIdx;		     // Transparency (index into transparency lookup table)
		/*0x16*/  s16    _animationIdx;		     // Texture animation (index into the texture animation lookup table) 

	};

	//
	struct colorDef
	{
		animblock	color;
		animblock	opacity;
	};

	struct transDef
	{
		animblock		trans;
	};

	struct texanimDef
	{
		animblock		trans;
		animblock		rot;
		animblock		scale;
	};

	enum BlendModes : int32_t
	{
		BM_OPAQUE,
		BM_ALPHA_TEST,
		BM_ALPHA_BLEND,
		BM_ADDITIVE_COLOR,
		BM_ADDITIVE_ALPHA,
		BM_MODULATE,
		BM_MODULATEX2,
		BM_MODULATEX4,
	};

	struct renderflag {
		u16 flags;
		//unsigned char f1;
		//unsigned char f2;
		u16 blend; 
	};

	struct ModelCameraDef {
		s32 id; // 0 is potrait camera, 1 characterinfo camera; -1 if none; referenced in CamLookup_Table
		float fov; // No radians, no degrees. Multiply by 35 to get degrees.
		float farclip; // Where it stops to be drawn.
		float nearclip; // Far and near. Both of them.
		animblock transPos; // (Vec3D) How the cameras position moves. Should be 3*3 floats. (? WoW parses 36 bytes = 3*3*sizeof(float))
		vector3df pos; // float, Where the camera is located.
		animblock transTarget; // (Vec3D) How the target moves. Should be 3*3 floats. (?)
		vector3df target; // float, Where the camera points to.
		animblock rot; // (Quat) The camera can have some roll-effect. Its 0 to 2*Pi.
	};

	struct ModelCameraDefV10 {
		s32 id; // 0 is potrait camera, 1 characterinfo camera; -1 if none; referenced in CamLookup_Table
		float farclip; // Where it stops to be drawn.
		float nearclip; // Far and near. Both of them.
		animblock transPos; // (Vec3D) How the cameras position moves. Should be 3*3 floats. (? WoW parses 36 bytes = 3*3*sizeof(float))
		vector3df pos; // float, Where the camera is located.
		animblock transTarget; // (Vec3D) How the target moves. Should be 3*3 floats. (?)
		vector3df target; // float, Where the camera points to.
		animblock rot; // (Quat) The camera can have some roll-effect. Its 0 to 2*Pi. 3 Floats!
		animblock fov; // (Float) One Float. cataclysm
	};

	struct FakeAnimationBlock {
		u32 nTimes;
		u32 ofsTimes;
		u32 nKeys;
		u32 ofsKeys;
	};

	struct ModelParticleParams {
		FakeAnimationBlock colors; 	// (Vec3D)	This one points to 3 floats defining red, green and blue.
		FakeAnimationBlock opacity;      // (UInt16)		Looks like opacity (short), Most likely they all have 3 timestamps for {start, middle, end}.
		FakeAnimationBlock sizes; 		// (Vec2D)	It carries two floats per key. (x and y scale)
		s32 d[2];
		FakeAnimationBlock Intensity; 	// (UInt16) Some kind of intensity values seen: 0,16,17,32(if set to different it will have high intensity) 
		FakeAnimationBlock unk2; 		// (UInt16)
		float unk[3];
		vector3df scales;
		float slowdown;
		float unknown1[2];
		float rotation;				//Sprite Rotation
		float unknown2[2];
		vector3df Rot1;					//Model Rotation 1
		vector3df Rot2;					//Model Rotation 2
		vector3df Trans;				//Model Translation
		float f2[4];
		s32 nUnknownReference;
		s32 ofsUnknownReferenc;
	};

#define	MODELPARTICLE_FLAGS_DONOTTRAIL		0x10
#define	MODELPARTICLE_FLAGS_DONOTBILLBOARD	0x1000
#define	MODELPARTICLE_EMITTER_PLANE			1
#define	MODELPARTICLE_EMITTER_SPHERE		2
#define	MODELPARTICLE_EMITTER_SPLINE		3

	struct ModelParticleEmitterDef {
		s32 id;
		s32 flags; // MODELPARTICLE_FLAGS_*
		vector3df pos; // The position. Relative to the following bone.
		s16 bone; // The bone its attached to.
		s16 texture; // And the texture that is used.
		s32 nModelFileName;
		s32 ofsModelFileName;
		s32 nParticleFileName;
		s32 ofsParticleFileName; // TODO

		s8 blend;
		s8 EmitterType; // EmitterType	 1 - Plane (rectangle), 2 - Sphere, 3 - Spline? (can't be bothered to find one)
		s16 ParticleColor; // This one is used so you can assign a color to specific particles. They loop over all 
		// particles and compare +0x2A to 11, 12 and 13. If that matches, the colors from the dbc get applied.
		s8 ParticleType; // 0 "normal" particle, 
		// 1 large quad from the particle's origin to its position (used in Moonwell water effects)
		// 2 seems to be the same as 0 (found some in the Deeprun Tram blinky-lights-sign thing)
		s8 HeaderTail; // 0 - Head, 1 - Tail, 2 - Both
		s16 TextureTileRotation; // TODO, Rotation for the texture tile. (Values: -1,0,1)

		s16 cols; // How many different frames are on that texture? People should learn what rows and cols are.
		s16 rows; // (2, 2) means slice texture to 2*2 pieces
		animblock EmissionSpeed; // (Float) All of the following blocks should be floats.
		animblock SpeedVariation; // (Float) Variation in the flying-speed. (range: 0 to 1)
		animblock VerticalRange; // (Float) Drifting away vertically. (range: 0 to pi)
		animblock HorizontalRange; // (Float) They can do it horizontally too! (range: 0 to 2*pi)
		animblock Gravity; // (Float) Fall down, apple!
		animblock Lifespan; // (Float) Everyone has to die.

		s32 unknown;

		animblock EmissionRate; // (Float) Stread your particles, emitter.

		s32 unknown2;

		animblock EmissionAreaLength; // (Float) Well, you can do that in this area.
		animblock EmissionAreaWidth; // (Float) 
		animblock Gravity2; // (Float) A second gravity? Its strong.
		ModelParticleParams p;
		animblock en; // (UInt16)
	};

	struct ModelParticleEmitterDefV10 {
		s32 id;
		s32 flags;
		vector3df pos; // The position. Relative to the following bone.
		s16 bone; // The bone its attached to.
		s16 texture; // And the texture that is used.
		s32 nModelFileName;
		s32 ofsModelFileName;
		s32 nParticleFileName;
		s32 ofsParticleFileName; // TODO
		s8 blend;
		s8 EmitterType; // EmitterType	 1 - Plane (rectangle), 2 - Sphere, 3 - Spline? (can't be bothered to find one)
		s16 ParticleColor; // This one is used so you can assign a color to specific particles. They loop over all 
		// particles and compare +0x2A to 11, 12 and 13. If that matches, the colors from the dbc get applied.
		s8 ParticleType; // 0 "normal" particle, 
		// 1 large quad from the particle's origin to its position (used in Moonwell water effects)
		// 2 seems to be the same as 0 (found some in the Deeprun Tram blinky-lights-sign thing)
		s8 HeaderTail; // 0 - Head, 1 - Tail, 2 - Both
		s16 TextureTileRotation; // TODO, Rotation for the texture tile. (Values: -1,0,1)
		s16 cols; // How many different frames are on that texture? People should learn what rows and cols are.
		s16 rows; // (2, 2) means slice texture to 2*2 pieces
		animblock EmissionSpeed; // (Float) All of the following blocks should be floats.
		animblock SpeedVariation; // (Float) Variation in the flying-speed. (range: 0 to 1)
		animblock VerticalRange; // (Float) Drifting away vertically. (range: 0 to pi)
		animblock HorizontalRange; // (Float) They can do it horizontally too! (range: 0 to 2*pi)
		animblock Gravity; // (Float) Fall down, apple!
		animblock Lifespan; // (Float) Everyone has to die.
		s32 unknown;
		animblock EmissionRate; // (Float) Stread your particles, emitter.
		s32 unknown2;
		animblock EmissionAreaLength; // (Float) Well, you can do that in this area.
		animblock EmissionAreaWidth; // (Float) 
		animblock Gravity2; // (Float) A second gravity? Its strong.
		ModelParticleParams p;
		animblock en; // (UInt16), seems unused in cataclysm
		s32 unknown3; // 12319, cataclysm
		s32 unknown4; // 12319, cataclysm
		s32 unknown5; // 12319, cataclysm
		s32 unknown6; // 12319, cataclysm
	};

	struct ModelRibbonEmitterDef {
		s32 id;
		s32 bone;
		vector3df pos;
		s32 nTextures;
		s32 ofsTextures;
		s32 nUnknown;
		s32 ofsUnknown;
		animblock color; // (Vec3D)
		animblock opacity; // (UInt16) And an alpha value in a short, where: 0 - transparent, 0x7FFF - opaque.
		animblock above; // (Float) The height above.
		animblock below; // (Float) The height below. Do not set these to the same!
		float res; // This defines how smooth the ribbon is. A low value may produce a lot of edges.
		float length; // The length aka Lifespan.
		float Emissionangle; // use arcsin(val) to get the angle in degree
		s16 s1, s2;
		animblock unk1; // (short)
		animblock unk2; // (boolean)

		s32 unknown; // This looks much like just some Padding to the fill up the 0x10 Bytes, always 0
	};

#	pragma pack ()

}