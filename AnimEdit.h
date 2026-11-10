// Animation editing helpers: undo/redo, frame range operations, keyframe navigation, backups.
#pragma once

class CSOEditDoc;

namespace AnimEdit
{
	// ---- undo / redo (automatic: any change to the loaded animation is detected and recorded)
	void Reset(CSOEditDoc* doc);          // call after an animation is loaded / created / closed
	void Poll(CSOEditDoc* doc);           // call regularly (timer); records finished edits
	bool Undo(CSOEditDoc* doc);
	bool Redo(CSOEditDoc* doc);
	bool CanUndo();
	bool CanRedo();
	bool IsDirty(CSOEditDoc* doc);        // changed since it was loaded / last saved
	void MarkSaved(CSOEditDoc* doc);

	// ---- frame operations (work on the timeline selection, or the current frame when nothing is selected)
	void DeleteSelection(CSOEditDoc* doc);
	void DuplicateSelection(CSOEditDoc* doc);
	void MoveSelection(CSOEditDoc* doc, int direction);   // -1 earlier, +1 later
	void SelectAll(CSOEditDoc* doc);
	void GoTo(CSOEditDoc* doc, int frame);

	// ---- keyframes of the selected bone
	bool IsKeyFrame(CSOEditDoc* doc, int frame);
	void JumpKey(CSOEditDoc* doc, int direction);          // -1 previous key, +1 next key

	// ---- convenience
	void CycleAnimation(CSOEditDoc* doc, int direction);   // next / previous .anm in the model's folder
	void SaveIfDirty(CSOEditDoc* doc);                    // silently saves a modified animation that already has a file
	void MakeBackup(const char* path);                     // copies path to path.bak before it is overwritten
}
