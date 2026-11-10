// Plain-English explanations shown as tooltips for the less obvious controls.
#include "stdafx.h"
#include "resource.h"
#include "ModernUI.h"

namespace
{
	struct Tip { UINT id; const wchar_t* text; };

	const Tip g_tips[] =
	{
		// ---- animation panel (bottom bar)
		{ IDC_BACK,   L"Previous frame  (Left arrow)" },
		{ IDC_PLAY,   L"Play the animation from the current frame  (Space)" },
		{ IDC_PAUSE,  L"Pause playback and stay on this frame  (Space)" },
		{ IDC_STOP,   L"Stop and go back to the first frame" },
		{ IDC_STEP,   L"Next frame  (Right arrow)" },
		{ IDC_ANIM_SLIDER, L"Timeline.\nClick or drag: scrub. Mouse wheel: step one frame. Double-click: edit that frame.\nShift+click/drag: select a range, then Delete / Shift+D (duplicate) / [ ] (move) it. Drag a selection to move it. Right-click for a menu.\nOrange diamonds mark frames where the selected bone changes - select a bone in the tree to see them." },
		{ IDC_SPEED, L"Playback speed multiplier (1.0 = normal). Mouse wheel or the arrows change it." },
		{ IDC_SPEED_CONTROL, L"Playback speed multiplier" },
		{ IDC_LOAD,  L"Load an animation file (.anm) for this model" },
		{ IDC_LOOP,  L"Repeat: restart from the first frame when the animation ends" },
		{ IDC_REVERSE_ANIM, L"Reverse the order of all frames in the loaded animation" },
		{ IDC_ANMBOX_INSERT_FRAME, L"Insert a copy of the current frame after it  (I). Hold the button to insert many." },
		{ IDC_ANMBOX_DELETE_FRAME, L"Delete the current frame  (Delete). Hold the button to delete many." },
		{ IDC_LERP_START_FRAME, L"First frame of the range to smooth out" },
		{ IDC_LERP_END_FRAME,   L"Last frame of the range to smooth out" },
		{ IDC_ANM_LERP_APPLY, L"Fill the frames between the two chosen frames with smooth in-between poses (linear interpolation). Selecting a range on the timeline fills in the two numbers for you." },
		{ IDC_ANMBOX_INHERIT_MATRIX, L"When on, a change you make to a bone in this frame is also applied to all frames after it" },
		{ IDC_ANMBOX_DROP_FIRST_FRAME, L"Leave out frame 1 when saving the .anm file (some game animations expect this)" },

		// ---- tool panel
		{ IDC_FINDING_COMPONENT, L"Type part of a bone, volume or mesh name and press Enter to jump to it in the tree" },
		{ IDC_MODELTREE, L"Model hierarchy.\nSkeleton = bones and their meshes, Volumes = collision shapes, Obstacles/Areas = map blockers.\nRight-click an entry for editing commands." },

		// ---- bone editor
		{ IDC_BONENAME, L"Unique name of this bone. Animations and volumes refer to bones by name." },
		{ IDC_PARENT_BONE, L"The bone this one is attached to. It moves, rotates and scales with its parent." },
		{ IDC_VOLUME_VIEW, L"The .ply mesh drawn on this bone (leave empty for an invisible helper bone)" },
		{ IDC_BROWSE, L"Choose the mesh (.ply) file" },
		{ IDC_ROUND_BONE_BUTTON, L"Round all matrix numbers to 4 decimals to remove floating-point noise" },
		{ IDC_MATRIX11, L"X axis of the bone: the direction (and scale) of its local X. Identity is 1 0 0 / 0 1 0 / 0 0 1." },
		{ IDC_MATRIX12, L"X axis of the bone: the direction (and scale) of its local X. Identity is 1 0 0 / 0 1 0 / 0 0 1." },
		{ IDC_MATRIX13, L"X axis of the bone: the direction (and scale) of its local X. Identity is 1 0 0 / 0 1 0 / 0 0 1." },
		{ IDC_MATRIX21, L"Y axis of the bone: the direction (and scale) of its local Y. Identity is 1 0 0 / 0 1 0 / 0 0 1." },
		{ IDC_MATRIX22, L"Y axis of the bone: the direction (and scale) of its local Y. Identity is 1 0 0 / 0 1 0 / 0 0 1." },
		{ IDC_MATRIX23, L"Y axis of the bone: the direction (and scale) of its local Y. Identity is 1 0 0 / 0 1 0 / 0 0 1." },
		{ IDC_MATRIX31, L"Z axis of the bone: the direction (and scale) of its local Z. Identity is 1 0 0 / 0 1 0 / 0 0 1." },
		{ IDC_MATRIX32, L"Z axis of the bone: the direction (and scale) of its local Z. Identity is 1 0 0 / 0 1 0 / 0 0 1." },
		{ IDC_MATRIX33, L"Z axis of the bone: the direction (and scale) of its local Z. Identity is 1 0 0 / 0 1 0 / 0 0 1." },
		{ IDC_MATRIX41, L"Position of the bone relative to its parent (X, Y, Z)" },
		{ IDC_MATRIX42, L"Position of the bone relative to its parent (X, Y, Z)" },
		{ IDC_MATRIX43, L"Position of the bone relative to its parent (X, Y, Z)" },
		{ IDC_VISIBLE, L"Draw this bone's mesh. Checked = visible, empty = hidden, filled square = use the parent's setting" },
		{ IDC_TERMINATOR, L"Marks the end of a kinematic chain (stops physics/IK from propagating further)" },
		{ IDC_GROUND, L"Bone is aligned to the ground (used for wheels, tracks and feet)" },
		{ IDC_LOD, L"This bone's mesh is a level-of-detail version, shown only at some zoom distances" },
		{ IDC_LOD_OFF, L"Switch this bone's mesh off at the chosen level of detail (it is not drawn)" },
		{ IDC_LOD_LAYER, L"Which detail level (0 = most detailed) this mesh belongs to" },
		{ IDC_NO_CAST_SHADOWS, L"This mesh does not cast shadows onto other objects" },
		{ IDC_NO_GET_SHADOWS, L"This mesh is not darkened by shadows from other objects" },
		{ IDC_NO_GROUP_MESH, L"Exclude this mesh from automatic mesh grouping (keeps it a separate draw call)" },
		{ IDC_DECAL_TARGET, L"Bullet holes, scorch marks and other decals may be painted onto this mesh" },
		{ IDC_RADIO1, L"Revolute joint: the bone rotates around an axis like a hinge (turret, door)" },
		{ IDC_RADIO2, L"Prismatic joint: the bone slides along an axis (recoil, hydraulic piston)" },
		{ IDC_RADIO3, L"Socket joint: the bone rotates freely in all directions like a ball joint" },
		{ IDC_RADIO4, L"No joint: the bone is rigidly fixed to its parent" },
		{ IDC_LIMITS, L"Restrict how far the joint may move" },
		{ IDC_LIMITS_COMMON, L"Symmetric limit: the joint may move this far in either direction" },
		{ IDC_LIMITS_LO, L"Lowest allowed value of the joint's movement" },
		{ IDC_LIMITS_HI, L"Highest allowed value of the joint's movement" },
		{ IDC_BONE_SPEED, L"How fast the joint is allowed to move (units per second, 0 = default)" },
		{ IDC_B_SPEED2, L"Use the secondary speed setting instead of the primary one" },
		{ IDC_COLORIZED, L"Paint this bone's mesh with a flat colour in the viewport to tell parts apart (editor only)" },
		{ IDC_BONE_COLOR_SEL, L"Pick the preview colour for this mesh" },
		{ IDC_PARAMETERS, L"Raw extra parameters written to the .mdl file for this bone. Only change this if you know the game's format." },
		{ IDC_BONE_SEQUENCE, L"Animation clips linked to this bone. The game plays them by name." },
		{ IDC_BONE_SEQUENCE_ADD, L"Link a new animation clip to this bone" },
		{ IDC_BONE_SEQUENCE_DELETE, L"Remove the selected animation clip from this bone" },
		{ IDC_BONE_SEQUENCE_AUTOSTART, L"Start this clip automatically when the object appears" },
		{ IDC_BONE_SEQUENCE_STORE, L"Remember the clip's position so it can be continued later" },
		{ IDC_BONE_SEQUENCE_RESUME, L"Continue from the stored position instead of starting over" },
		{ IDC_BONE_SEQUENCE_SMOOTH, L"Blend time when switching from another clip, in seconds" },
		{ IDC_BONE_SEQUENCE_SPEED, L"Playback speed of this clip (1 = normal)" },
		{ IDC_BONE_SEQUENCE_FILE, L"The .anm file this clip plays" },
		{ IDC_BONE_WRENCH, L"Open helper tools for the parameter text" },

		// ---- volume editor
		{ IDC_VOLNAME, L"Name of this collision volume" },
		{ IDC_VOL_BONES, L"Bones in the model. Pick the one this volume should follow." },
		{ IDC_VOL_BONE, L"The bone this volume is attached to - the volume moves and rotates with it" },
		{ IDC_VOLTYPE1, L"Box volume: a rectangular collision box" },
		{ IDC_VOLTYPE2, L"Cylinder volume: good for wheels, barrels and round turrets" },
		{ IDC_VOLTYPE3, L"Polyhedron volume: a custom shape loaded from a .vol file" },
		{ IDC_VP_APPLY, L"Apply the changes to the selected volume" },
		{ IDC_ROUND_VOLUME_BUTTON, L"Round all matrix numbers to 4 decimals" },
		{ IDC_BOX_LENGTH, L"Box size along its X axis" },
		{ IDC_BOX_WIDTH,  L"Box size along its Y axis" },
		{ IDC_BOX_HEIGHT, L"Box size along its Z axis" },
		{ IDC_BOX_FIT, L"Resize the box to wrap the mesh of the attached bone" },
		{ IDC_CYL_LENGTH, L"Cylinder length along its axis" },
		{ IDC_CYL_DIAMETER, L"Cylinder diameter" },
		{ IDC_CYL_FIT, L"Resize the cylinder to wrap the mesh of the attached bone" },
		{ IDC_XYZ1, L"Cylinder runs along the X axis" },
		{ IDC_XYZ2, L"Cylinder runs along the Y axis" },
		{ IDC_XYZ3, L"Cylinder runs along the Z axis" },
		{ IDC_VOL_POLY_NAME, L"The .vol file that defines this volume's shape" },
		{ IDC_VOL_CHOOSE, L"Choose the .vol file" },

		{ IDC_VOL_TAB, L"Shape of this collision volume: Box, Cylinder, or Polyhedron (a custom shape from a .vol file)" },
		{ IDC_VOL_M1_1, L"X axis of the volume (direction and scale)" },
		{ IDC_VOL_M1_2, L"X axis of the volume (direction and scale)" },
		{ IDC_VOL_M1_3, L"X axis of the volume (direction and scale)" },
		{ IDC_VOL_M2_1, L"Y axis of the volume (direction and scale)" },
		{ IDC_VOL_M2_2, L"Y axis of the volume (direction and scale)" },
		{ IDC_VOL_M2_3, L"Y axis of the volume (direction and scale)" },
		{ IDC_VOL_M3_1, L"Z axis of the volume (direction and scale)" },
		{ IDC_VOL_M3_2, L"Z axis of the volume (direction and scale)" },
		{ IDC_VOL_M3_3, L"Z axis of the volume (direction and scale)" },
		{ IDC_VOL_M4_1, L"Position of the volume relative to its bone (X, Y, Z)" },
		{ IDC_VOL_M4_2, L"Position of the volume relative to its bone (X, Y, Z)" },
		{ IDC_VOL_M4_3, L"Position of the volume relative to its bone (X, Y, Z)" },

		// ---- animation data dialog
		{ IDC_MBONES, L"Bones that have movement data in this animation. Select one to edit its transform on the current frame." },
		{ IDC_VISI, L"Whether this bone is visible during this frame" },
		{ IDC_INSERT_FRAME, L"Insert a copy of the current frame" },
		{ IDC_DELETE_FRAME, L"Delete the current frame" },
		{ IDC_DELETE_ANM_BONE_NAME, L"Remove the selected bone from the whole animation" },
		{ IDC_ANM_BONE_CLONE, L"Duplicate the selected bone's animation under a new name" },
		{ IDC_ANM_BONE_EXPORT, L"Save the selected bone's animation to a file" },
		{ IDC_ANM_BONE_IMPORT, L"Load a bone animation from a file" },
		{ IDC_RESET_BONE, L"Reset the selected bone to its position in the model on this frame" },
		{ IDC_MATRIX_REWRITE, L"Overwrite the stored matrix with the numbers typed on the left" },
		{ IDC_ANM_AXIS_X, L"Apply the effect to the X axis" },
		{ IDC_ANM_AXIS_Y, L"Apply the effect to the Y axis" },
		{ IDC_ANM_AXIS_Z, L"Apply the effect to the Z axis" },

		// ---- preferences / setup
		{ IDC_RESPATH,  L"Folder of the game, the one that contains the 'resource' folder" },
		{ IDC_RESPATH2, L"Mod folder searched for models and textures (optional)" },
		{ IDC_RESPATH3, L"Mod folder searched for models and textures (optional)" },
		{ IDC_RESPATH4, L"Mod folder searched for models and textures (optional)" },
		{ IDC_RESPATH5, L"Mod folder searched for models and textures (optional)" },
		{ IDC_RESPATH6, L"Mod folder searched for models and textures (optional)" },
		{ IDC_YOUR_NAME, L"Your name, written into the file header of models you save" },
		{ IDC_FOV_EDIT, L"Field of view of the 3D view in degrees (smaller = more zoomed in)" },
		{ IDC_VIEW_LAMP_MODEL, L"Show a light-bulb model where the scene light is" },
		{ IDC_VIEW_RAM_VOL, L"Show the ram (collision pusher) volumes of vehicles" },
		{ IDC_VIEW_TRACK_VOL, L"Show the track volumes of vehicles" },

		// ---- colours dialog
		{ IDC_ELEMENTS, L"Pick which part of the viewports to recolour" },
		{ IDC_COLOR_SEL, L"Choose a new colour for the selected element" },
	};
}

namespace ModernUI
{
	const wchar_t* TooltipFor(UINT id)
	{
		for (size_t i = 0; i < sizeof(g_tips) / sizeof(g_tips[0]); i++)
			if (g_tips[i].id == id) return g_tips[i].text;
		return NULL;
	}
}
