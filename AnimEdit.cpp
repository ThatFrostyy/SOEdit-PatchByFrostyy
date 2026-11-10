// Animation editing helpers: undo/redo, frame range operations, keyframe navigation, backups.
#include "stdafx.h"
#include "SOEdit.h"
#include "MainFrm.h"
#include "SOEditDoc.h"
#include "CAnimFrame.h"
#include "ModernUI.h"
#include "AnimEdit.h"
#include "resource.h"

#include <vector>
#include <string>
#include <algorithm>
#undef min
#undef max

extern char g_PendingAnmPath[_MAX_PATH];

namespace AnimEdit
{
namespace
{
	struct Snap
	{
		std::vector<BYTE> data;
		int frame;
		unsigned hash;
		Snap() : frame(0), hash(0) {}
	};

	std::vector<Snap> g_undo, g_redo;
	Snap      g_base;
	bool      g_haveBase = false;
	void*     g_seen = NULL;          // animation object the baseline belongs to
	unsigned  g_savedHash = 0;
	bool      g_pendingSaved = false;   // the loader may still tweak the data right after loading

	unsigned Mix(unsigned h, const void* p, size_t n)
	{
		const BYTE* b = (const BYTE*)p;
		for (size_t i = 0; i < n; i++) { h ^= b[i]; h *= 16777619u; }
		return h;
	}

	unsigned HashAnim(CAnimBone* a)
	{
		unsigned h = 2166136261u;
		if (!a) return h;
		h = Mix(h, &a->m_FrameCnt, sizeof(int));
		h = Mix(h, &a->m_BoneCnt, sizeof(int));
		for (int i = 0; i < a->m_BoneCnt; i++)
			if (a->m_BoneMap && a->m_BoneMap[i]) h = Mix(h, a->m_BoneMap[i], strlen(a->m_BoneMap[i]) + 1);
		for (int f = 0; f < a->m_FrameCnt && a->m_Frames; f++)
		{
			CAnimFrame& fr = a->m_Frames[f];
			h = Mix(h, &fr.m_Bone, sizeof(int));
			h = Mix(h, &fr.m_Matrix34, sizeof(matrix34_t));
			for (CAnimSub* s = fr.m_head; s; s = s->m_next)
			{
				h = Mix(h, &s->m_Bone, sizeof(int));
				h = Mix(h, &s->m_Matrix34, sizeof(matrix34_t));
				h = Mix(h, &s->m_Visible, sizeof(int));
				h = Mix(h, &s->chunk_flags, sizeof(short));
			}
		}
		return h;
	}

	CString TempFile()
	{
		char dir[MAX_PATH], name[MAX_PATH];
		GetTempPathA(MAX_PATH, dir);
		GetTempFileNameA(dir, "soe", 0, name);
		return name;
	}

	bool Capture(CSOEditDoc* doc, Snap& s)
	{
		if (!doc || !doc->m_AnimBone) return false;
		CString path = TempFile();
		FILE* fp = fopen(path, "wb");
		if (!fp) return false;
		doc->m_AnimBone->ProcessSave(fp);
		fclose(fp);
		fp = fopen(path, "rb");
		if (!fp) { DeleteFileA(path); return false; }
		fseek(fp, 0, SEEK_END);
		long n = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		s.data.resize(n > 0 ? (size_t)n : 0);
		if (n > 0) fread(&s.data[0], 1, (size_t)n, fp);
		fclose(fp);
		DeleteFileA(path);
		s.frame = doc->m_Frame;
		s.hash = HashAnim(doc->m_AnimBone);
		return n > 0;
	}

	CAnimBone* Rebuild(const Snap& s)
	{
		CString path = TempFile();
		FILE* fp = fopen(path, "wb");
		if (!fp) return NULL;
		fwrite(&s.data[0], 1, s.data.size(), fp);
		fclose(fp);
		bool ok = false;
		CAnimBone* a = new CAnimBone((char*)(LPCSTR)path, ok);
		DeleteFileA(path);
		if (!ok) { delete a; return NULL; }
		return a;
	}

	void Refresh(CSOEditDoc* doc)
	{
		doc->AnimBoxUPD();
		doc->OnModEditing();
		doc->UpdateAllViews(NULL);
	}

	void Range(CSOEditDoc* doc, int& a, int& b)
	{
		ModernUI::GetTimelineSelection(&a, &b);
		int last = doc->m_AnimBone ? doc->m_AnimBone->m_FrameCnt - 1 : 0;
		if (a < 0 || b < 0) { a = b = doc->m_Frame; }
		if (a > b) std::swap(a, b);
		if (a < 0) a = 0;
		if (b > last) b = last;
		if (a > b) a = b;
	}

	bool AnimReady(CSOEditDoc* doc)
	{
		return doc && doc->m_AnimBone && doc->m_AnimBone->m_FrameCnt > 0 && !doc->m_Animate;
	}
}

// ------------------------------------------------------------------ undo / redo
void Reset(CSOEditDoc* doc)
{
	g_undo.clear(); g_redo.clear();
	g_haveBase = false; g_seen = NULL;
	ModernUI::SetTimelineSelection(-1, -1);
	if (doc && doc->m_AnimBone)
	{
		g_haveBase = Capture(doc, g_base);
		g_seen = doc->m_AnimBone;
		g_savedHash = g_base.hash;
		g_pendingSaved = true;
	}
}

void Poll(CSOEditDoc* doc)
{
	if (!doc || !doc->m_AnimBone) { if (g_haveBase) Reset(doc); return; }
	if (!g_haveBase || g_seen != doc->m_AnimBone) { Reset(doc); return; }
	if (g_pendingSaved)
	{
		g_pendingSaved = false;                              // settle: whatever the loader did counts as the saved state
		Capture(doc, g_base);
		g_savedHash = g_base.hash;
		return;
	}
	{
		// A selected range of 2+ frames pre-fills the "Lerp frames .. to .." boxes.
		static int lastA = -2, lastB = -2;
		int sa, sb; ModernUI::GetTimelineSelection(&sa, &sb);
		if ((sa != lastA || sb != lastB) && sa >= 0 && sb >= 0 && sa != sb)
		{
			CMainFrame* fr = (CMainFrame*)AfxGetMainWnd();
			CWnd* e1 = fr ? fr->m_wndAnimBox.GetDlgItem(IDC_LERP_START_FRAME) : NULL;
			CWnd* e2 = fr ? fr->m_wndAnimBox.GetDlgItem(IDC_LERP_END_FRAME) : NULL;
			if (e1 && e2)
			{
				char t[16];
				sprintf(t, "%d", std::min(sa, sb) + 1); e1->SetWindowText(t);
				sprintf(t, "%d", std::max(sa, sb) + 1); e2->SetWindowText(t);
			}
		}
		lastA = sa; lastB = sb;
	}
	if (GetKeyState(VK_LBUTTON) & 0x8000) return;          // wait until a drag has finished
	unsigned h = HashAnim(doc->m_AnimBone);
	if (h == g_base.hash) return;
	if (g_undo.size() >= 100) g_undo.erase(g_undo.begin());
	g_undo.push_back(g_base);
	g_redo.clear();
	Capture(doc, g_base);
}

bool CanUndo() { return !g_undo.empty(); }
bool CanRedo() { return !g_redo.empty(); }

bool Undo(CSOEditDoc* doc)
{
	if (!AnimReady(doc)) return false;
	Poll(doc);                                              // commit anything still pending
	if (g_undo.empty()) return false;
	Snap target = g_undo.back();
	CAnimBone* a = Rebuild(target);
	if (!a) return false;
	g_undo.pop_back();
	Snap cur; Capture(doc, cur);
	g_redo.push_back(cur);
	delete doc->m_AnimBone;
	doc->m_AnimBone = a;
	doc->m_Frame = std::min(target.frame, a->m_FrameCnt - 1);
	target.hash = HashAnim(a);          // the rebuilt copy may hash differently from the original object
	g_base = target; g_haveBase = true; g_seen = a;
	ModernUI::SetTimelineSelection(-1, -1);
	Refresh(doc);
	return true;
}

bool Redo(CSOEditDoc* doc)
{
	if (!AnimReady(doc)) return false;
	Poll(doc);
	if (g_redo.empty()) return false;
	Snap target = g_redo.back();
	CAnimBone* a = Rebuild(target);
	if (!a) return false;
	g_redo.pop_back();
	Snap cur; Capture(doc, cur);
	g_undo.push_back(cur);
	delete doc->m_AnimBone;
	doc->m_AnimBone = a;
	doc->m_Frame = std::min(target.frame, a->m_FrameCnt - 1);
	target.hash = HashAnim(a);          // the rebuilt copy may hash differently from the original object
	g_base = target; g_haveBase = true; g_seen = a;
	ModernUI::SetTimelineSelection(-1, -1);
	Refresh(doc);
	return true;
}

bool IsDirty(CSOEditDoc* doc)
{
	if (!doc || !doc->m_AnimBone || g_pendingSaved) return false;
	return HashAnim(doc->m_AnimBone) != g_savedHash;
}

void MarkSaved(CSOEditDoc* doc)
{
	if (doc && doc->m_AnimBone) g_savedHash = HashAnim(doc->m_AnimBone);
}

// ------------------------------------------------------------------ frame operations
void GoTo(CSOEditDoc* doc, int frame)
{
	if (!AnimReady(doc)) return;
	int last = doc->m_AnimBone->m_FrameCnt - 1;
	doc->m_Frame = std::max(0, std::min(frame, last));
	Refresh(doc);
}

void DeleteSelection(CSOEditDoc* doc)
{
	if (!AnimReady(doc)) return;
	Poll(doc);
	int a, b; Range(doc, a, b);
	int total = doc->m_AnimBone->m_FrameCnt;
	int count = b - a + 1;
	if (count >= total) { count = total - 1; }              // an animation always keeps one frame
	for (int i = 0; i < count; i++) doc->m_AnimBone->FrameDelete(a);
	doc->m_Frame = std::max(0, std::min(a, doc->m_AnimBone->m_FrameCnt - 1));
	ModernUI::SetTimelineSelection(-1, -1);
	Refresh(doc);
}

void DuplicateSelection(CSOEditDoc* doc)
{
	if (!AnimReady(doc)) return;
	Poll(doc);
	int a, b; Range(doc, a, b);
	int n = b - a + 1;
	for (int k = 0; k < n; k++) doc->m_AnimBone->InsertClonedFrame(a + k, b + 1 + k);
	doc->m_Frame = b + 1;
	ModernUI::SetTimelineSelection(b + 1, b + n);
	Refresh(doc);
}

void MoveSelection(CSOEditDoc* doc, int direction)
{
	if (!AnimReady(doc)) return;
	Poll(doc);
	int a, b; Range(doc, a, b);
	int last = doc->m_AnimBone->m_FrameCnt - 1;
	bool sel = false; { int x, y; ModernUI::GetTimelineSelection(&x, &y); sel = (x >= 0 && y >= 0); }
	if (direction < 0 && a > 0)
	{
		doc->m_AnimBone->MoveFrame(a - 1, b);
		a--; b--; doc->m_Frame = std::max(0, doc->m_Frame - 1);
	}
	else if (direction > 0 && b < last)
	{
		doc->m_AnimBone->MoveFrame(b + 1, a);
		a++; b++; doc->m_Frame = std::min(last, doc->m_Frame + 1);
	}
	else return;
	if (sel) ModernUI::SetTimelineSelection(a, b); else doc->m_Frame = a;
	Refresh(doc);
}

void SelectAll(CSOEditDoc* doc)
{
	if (!AnimReady(doc)) return;
	ModernUI::SetTimelineSelection(0, doc->m_AnimBone->m_FrameCnt - 1);
}

// ------------------------------------------------------------------ keyframes
bool IsKeyFrame(CSOEditDoc* doc, int frame)
{
	if (!doc || !doc->m_AnimBone || !doc->m_SelBone || !doc->m_AnimBone->m_Frames) return false;
	CAnimBone* anm = doc->m_AnimBone;
	if (frame < 0 || frame >= anm->m_FrameCnt) return false;
	int bone = -1;
	for (int i = 0; i < anm->m_BoneCnt; i++)
		if (anm->m_BoneMap[i] && _stricmp(anm->m_BoneMap[i], doc->m_SelBone) == 0) { bone = i; break; }
	if (bone < 0) return false;
	CAnimSub* cur = anm->m_Frames[frame].FindSub(bone);
	if (frame == 0) return cur != NULL;
	CAnimSub* prev = anm->m_Frames[frame - 1].FindSub(bone);
	if (!cur || !prev) return (cur != NULL) != (prev != NULL);
	return memcmp(&cur->m_Matrix34, &prev->m_Matrix34, sizeof(matrix34_t)) != 0;
}

void JumpKey(CSOEditDoc* doc, int direction)
{
	if (!AnimReady(doc)) return;
	int last = doc->m_AnimBone->m_FrameCnt - 1;
	for (int f = doc->m_Frame + direction; f >= 0 && f <= last; f += direction)
		if (IsKeyFrame(doc, f)) { GoTo(doc, f); return; }
}

// ------------------------------------------------------------------ convenience
void MakeBackup(const char* path)
{
	if (!path || !*path) return;
	DWORD attr = GetFileAttributesA(path);
	if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY)) return;
	CString bak = CString(path) + ".bak";
	CopyFileA(path, bak, FALSE);
}

void SaveIfDirty(CSOEditDoc* doc)
{
	if (!doc || !doc->m_AnimBone || !doc->szAnimPath[0] || !IsDirty(doc)) return;
	if (GetFileAttributesA(doc->szAnimPath) == INVALID_FILE_ATTRIBUTES) return;   // never saved: use Animation > Save
	MakeBackup(doc->szAnimPath);
	FILE* fp = fopen(doc->szAnimPath, "wb");
	if (!fp) return;
	doc->m_AnimBone->ProcessSave(fp);
	fclose(fp);
	MarkSaved(doc);
}

void CycleAnimation(CSOEditDoc* doc, int direction)
{
	CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();
	if (!doc || !frame || !strlen(frame->m_CurMdlPath)) return;
	CString folder = doc->szAnimPath[0] ? CString(doc->szAnimPath) : CString(frame->m_CurMdlPath);
	folder.Replace('\\', '/');
	int slash = folder.ReverseFind('/');
	if (slash < 0) return;
	CString dir = folder.Left(slash + 1);
	std::vector<std::string> files;
	WIN32_FIND_DATAA fd;
	HANDLE h = FindFirstFileA(dir + "*.anm", &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do { files.push_back(fd.cFileName); } while (FindNextFileA(h, &fd));
		FindClose(h);
	}
	if (files.empty()) return;
	std::sort(files.begin(), files.end(), [](const std::string& x, const std::string& y) { return _stricmp(x.c_str(), y.c_str()) < 0; });
	int cur = -1;
	CString curName = doc->szAnimPath[0] ? CString(doc->szAnimPath).Mid(CString(doc->szAnimPath).ReverseFind('/') + 1) : CString();
	for (size_t i = 0; i < files.size(); i++)
		if (_stricmp(files[i].c_str(), curName) == 0) { cur = (int)i; break; }
	int n = (int)files.size();
	int next = cur < 0 ? (direction > 0 ? 0 : n - 1) : (cur + direction + n) % n;
	if (IsDirty(doc))
	{
		if (MessageBoxA(frame->m_hWnd, "This animation has unsaved changes.\nLoad another animation and discard them?",
			"Unsaved animation", MB_YESNO | MB_ICONQUESTION) != IDYES) return;
	}
	CString full = dir + files[next].c_str();
	strncpy(g_PendingAnmPath, full, _MAX_PATH - 1);
	frame->SendMessage(WM_COMMAND, ID_ANM_LOAD_AL, 0);
}

} // namespace AnimEdit
