// ModernUI - dark theme, custom controls and OpenGL viewport helpers for SOEdit.
// Written by Frostyy's patch. Self contained: only needs Win32 + comctl32 v6 + OpenGL.
#pragma once

#include <windows.h>

namespace ModernUI
{
	struct Palette
	{
		COLORREF bg;       // window / dialog background
		COLORREF bgAlt;    // toolbars, status bar, tab strip
		COLORREF ctrl;     // edit boxes, lists, trees
		COLORREF border;   // 1px borders and separators
		COLORREF hot;      // hover background
		COLORREF text;     // primary text
		COLORREF dim;      // secondary / disabled text
		COLORREF accent;   // selection, check marks, focus
	};

	const Palette& P();

	// Call once at the very start of InitInstance (before any window exists).
	void Init();
	// Call from ExitInstance.
	void Shutdown();

	// Skin a single top level window (dark caption, rounded corners, menu bar painting).
	void SkinTopLevel(HWND hwnd);

	// Re-skins a MFC toolbar: converts the classic grey-background bitmap to a crisp,
	// scaled, alpha-keyed image list with dark-theme friendly outlines.
	void ModernizeToolbar(HWND hToolbar, UINT bitmapResId);

	// Fullscreen vertical gradient for the OpenGL viewport. Call right after glClear.
	void DrawGradientBackground(const unsigned char bottomRgb[3]);
	// Soft vignette-free ground "horizon" glow helper colours (0..1 range).
	void LerpColor(const unsigned char a[3], const unsigned char b[3], float t, float out[3]);
}
