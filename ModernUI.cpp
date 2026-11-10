// ModernUI - dark theme, custom controls and OpenGL viewport helpers for SOEdit.
#include "stdafx.h"
#include "ModernUI.h"

#include <windowsx.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <GL/gl.h>
#include <vector>
#include "MainFrm.h"

#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "comctl32.lib")
#if defined(_M_IX86)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined(_M_X64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

namespace ModernUI
{
namespace
{
	const Palette g_pal =
	{
		RGB(30, 31, 37),    // bg
		RGB(37, 38, 46),    // bgAlt
		RGB(45, 47, 57),    // ctrl
		RGB(66, 69, 82),    // border
		RGB(58, 61, 74),    // hot
		RGB(230, 232, 238), // text
		RGB(140, 145, 162), // dim
		RGB(94, 142, 255)   // accent
	};

	HHOOK   g_hook = NULL;
	HBRUSH  g_brBg = NULL, g_brAlt = NULL, g_brCtrl = NULL;
	HFONT   g_uiFont = NULL;
	bool    g_darkApi = false;

	const UINT_PTR ID_DLG = 0x5E01, ID_BTN = 0x5E02, ID_TAB = 0x5E03, ID_SBAR = 0x5E04,
	               ID_TBAR = 0x5E05, ID_FRAME = 0x5E06;

	// ------------------------------------------------------------------ uxtheme dark mode (undocumented ordinals)
	enum PreferredAppMode { PAM_Default, PAM_AllowDark, PAM_ForceDark, PAM_ForceLight };
	typedef PreferredAppMode (WINAPI *fnSetPreferredAppMode)(PreferredAppMode);
	typedef void (WINAPI *fnFlushMenuThemes)();
	typedef BOOL (WINAPI *fnAllowDarkModeForWindow)(HWND, BOOL);
	typedef UINT (WINAPI *fnGetDpiForWindow)(HWND);
	fnAllowDarkModeForWindow pAllowDarkModeForWindow = NULL;
	fnGetDpiForWindow        pGetDpiForWindow = NULL;

	int Scale(HWND h, int px)
	{
		UINT dpi = 96;
		if (pGetDpiForWindow && h)
			dpi = pGetDpiForWindow(h);
		if (!dpi) dpi = 96;
		return MulDiv(px, (int)dpi, 96);
	}

	bool ClassIs(HWND h, const wchar_t* name)
	{
		wchar_t cls[64] = L"";
		GetClassNameW(h, cls, 64);
		return _wcsicmp(cls, name) == 0;
	}

	void Theme(HWND h, const wchar_t* name)
	{
		if (pAllowDarkModeForWindow) pAllowDarkModeForWindow(h, TRUE);
		SetWindowTheme(h, name, NULL);
	}

	// ------------------------------------------------------------------ tiny double buffer
	struct Buffered
	{
		HDC dc, mem; HBITMAP bmp, old; RECT rc;
		Buffered(HDC target, const RECT& r) : dc(target), rc(r)
		{
			mem = CreateCompatibleDC(dc);
			bmp = CreateCompatibleBitmap(dc, r.right - r.left, r.bottom - r.top);
			old = (HBITMAP)SelectObject(mem, bmp);
			SetViewportOrgEx(mem, -r.left, -r.top, NULL);
		}
		~Buffered()
		{
			SetViewportOrgEx(mem, 0, 0, NULL);
			BitBlt(dc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, mem, 0, 0, SRCCOPY);
			SelectObject(mem, old); DeleteObject(bmp); DeleteDC(mem);
		}
	};

	void Fill(HDC dc, const RECT& r, COLORREF c)
	{
		SetBkColor(dc, c);
		ExtTextOutW(dc, 0, 0, ETO_OPAQUE, &r, NULL, 0, NULL);
	}

	HFONT ControlFont(HWND h)
	{
		HFONT f = (HFONT)SendMessage(h, WM_GETFONT, 0, 0);
		return f ? f : g_uiFont;
	}

	// MFC control bars paint their non-client border strip with COLOR_BTNFACE; repaint it dark.
	void PaintNcDark(HWND h, COLORREF c)
	{
		HDC dc = GetWindowDC(h);
		if (!dc) return;
		RECT wr, cr; GetWindowRect(h, &wr); GetClientRect(h, &cr);
		POINT o = { 0, 0 }; ClientToScreen(h, &o);
		OffsetRect(&cr, o.x - wr.left, o.y - wr.top);
		RECT all = { 0, 0, wr.right - wr.left, wr.bottom - wr.top };
		ExcludeClipRect(dc, cr.left, cr.top, cr.right, cr.bottom);
		Fill(dc, all, c);
		ReleaseDC(h, dc);
	}

	bool IsControlBar(HWND h)
	{
		wchar_t cls[64] = L"";
		GetClassNameW(h, cls, 64);
		return !_wcsnicmp(cls, L"AfxControlBar", 13);
	}

	// ------------------------------------------------------------------ dialogs and dialog bars
	LRESULT CALLBACK DlgProc(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR id, DWORD_PTR)
	{
		switch (m)
		{
		case WM_CTLCOLORDLG:
			return (LRESULT)g_brBg;
		case WM_CTLCOLORSTATIC:
		{
			HDC dc = (HDC)w; HWND ctl = (HWND)l;
			SetTextColor(dc, IsWindowEnabled(ctl) ? g_pal.text : g_pal.dim);
			if (ClassIs(ctl, L"Edit")) { SetBkColor(dc, g_pal.ctrl); return (LRESULT)g_brCtrl; }
			SetBkMode(dc, TRANSPARENT);
			SetBkColor(dc, g_pal.bg);
			return (LRESULT)g_brBg;
		}
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		{
			HDC dc = (HDC)w;
			SetTextColor(dc, g_pal.text);
			SetBkColor(dc, g_pal.ctrl);
			return (LRESULT)g_brCtrl;
		}
		case WM_CTLCOLORBTN:
		{
			HDC dc = (HDC)w;
			SetTextColor(dc, g_pal.text);
			SetBkColor(dc, g_pal.bg);
			return (LRESULT)g_brBg;
		}
		case WM_ERASEBKGND:
		{
			RECT rc; GetClientRect(h, &rc);
			Fill((HDC)w, rc, g_pal.bg);
			return 1;
		}
		case WM_NCPAINT:
			if (IsControlBar(h)) { PaintNcDark(h, g_pal.bg); return 0; }
			break;
		case WM_NCDESTROY:
			RemoveWindowSubclass(h, DlgProc, id);
			break;
		}
		return DefSubclassProc(h, m, w, l);
	}

	BOOL CALLBACK RecolorChild(HWND c, LPARAM)
	{
		if (ClassIs(c, L"SysTreeView32"))
		{
			TreeView_SetBkColor(c, g_pal.ctrl);
			TreeView_SetTextColor(c, g_pal.text);
			TreeView_SetLineColor(c, g_pal.border);
			InvalidateRect(c, NULL, TRUE);
		}
		else if (ClassIs(c, L"SysListView32"))
		{
			ListView_SetBkColor(c, g_pal.ctrl);
			ListView_SetTextBkColor(c, g_pal.ctrl);
			ListView_SetTextColor(c, g_pal.text);
			InvalidateRect(c, NULL, TRUE);
		}
		return TRUE;
	}

	void SkinDialogLike(HWND h)
	{
		EnumChildWindows(h, RecolorChild, 0);
		if (!h || GetWindowSubclass(h, DlgProc, ID_DLG, NULL)) return;
		SetWindowSubclass(h, DlgProc, ID_DLG, 0);
		InvalidateRect(h, NULL, TRUE);
	}

	// ------------------------------------------------------------------ check boxes, radio buttons, group boxes
	struct BtnState { bool hot; bool tracking; };

	void PaintButton(HWND h, HDC dc, bool hot)
	{
		RECT rc; GetClientRect(h, &rc);
		LONG style = GetWindowLong(h, GWL_STYLE);
		UINT type = style & BS_TYPEMASK;
		bool enabled = IsWindowEnabled(h) != 0;
		LRESULT st = SendMessage(h, BM_GETSTATE, 0, 0);
		bool pushed = (st & BST_PUSHED) != 0;
		bool focus = (st & BST_FOCUS) != 0;
		int chk = (int)SendMessage(h, BM_GETCHECK, 0, 0);
		HFONT oldF = (HFONT)SelectObject(dc, ControlFont(h));
		wchar_t txt[512]; txt[0] = 0;
		GetWindowTextW(h, txt, 512);
		SetBkMode(dc, TRANSPARENT);

		if (type == BS_GROUPBOX)
		{
			TEXTMETRIC tm; GetTextMetrics(dc, &tm);
			RECT tr = { Scale(h, 10), 0, rc.right - Scale(h, 10), tm.tmHeight };
			RECT calc = tr;
			DrawTextW(dc, txt, -1, &calc, DT_LEFT | DT_SINGLELINE | DT_CALCRECT);
			int top = tm.tmHeight / 2;
			HPEN pen = CreatePen(PS_SOLID, 1, g_pal.border);
			HPEN op = (HPEN)SelectObject(dc, pen);
			HBRUSH ob = (HBRUSH)SelectObject(dc, GetStockObject(NULL_BRUSH));
			RoundRect(dc, 0, top, rc.right, rc.bottom, Scale(h, 8), Scale(h, 8));
			SelectObject(dc, ob); SelectObject(dc, op); DeleteObject(pen);
			if (txt[0])
			{
				RECT gap = { tr.left - 4, 0, tr.left + (calc.right - calc.left) + 4, tm.tmHeight };
				Fill(dc, gap, g_pal.bg);
				SetTextColor(dc, enabled ? g_pal.text : g_pal.dim);
				DrawTextW(dc, txt, -1, &tr, DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
			}
			SelectObject(dc, oldF);
			return;
		}

		Fill(dc, rc, g_pal.bg);
		bool radio = (type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON);
		int box = Scale(h, 15);
		int by = (rc.bottom - rc.top - box) / 2;
		RECT br = { 1, by, 1 + box, by + box };

		COLORREF border = !enabled ? g_pal.border : (chk ? g_pal.accent : (hot || pushed ? g_pal.accent : RGB(104, 109, 128)));
		COLORREF fillc = !enabled ? g_pal.bgAlt : (chk == 1 ? g_pal.accent : (chk == 2 ? g_pal.accent : (pushed ? g_pal.hot : g_pal.ctrl)));
		if (radio) fillc = !enabled ? g_pal.bgAlt : (pushed ? g_pal.hot : g_pal.ctrl);
		HPEN pen = CreatePen(PS_SOLID, 1, border);
		HBRUSH br2 = CreateSolidBrush(fillc);
		HPEN op = (HPEN)SelectObject(dc, pen);
		HBRUSH ob = (HBRUSH)SelectObject(dc, br2);
		if (radio) Ellipse(dc, br.left, br.top, br.right, br.bottom);
		else       RoundRect(dc, br.left, br.top, br.right, br.bottom, Scale(h, 5), Scale(h, 5));
		SelectObject(dc, ob); SelectObject(dc, op); DeleteObject(br2); DeleteObject(pen);

		if (radio && chk)
		{
			int in = Scale(h, 4);
			HBRUSH ab = CreateSolidBrush(enabled ? g_pal.accent : g_pal.dim);
			HPEN np = (HPEN)SelectObject(dc, GetStockObject(NULL_PEN));
			HBRUSH ob2 = (HBRUSH)SelectObject(dc, ab);
			Ellipse(dc, br.left + in, br.top + in, br.right - in + 1, br.bottom - in + 1);
			SelectObject(dc, ob2); SelectObject(dc, np); DeleteObject(ab);
		}
		else if (!radio && chk == 1)
		{
			HPEN cp = CreatePen(PS_SOLID, Scale(h, 2), enabled ? RGB(255, 255, 255) : g_pal.dim);
			HPEN o2 = (HPEN)SelectObject(dc, cp);
			int x = br.left, y = br.top, s = box;
			MoveToEx(dc, x + s * 22 / 100, y + s * 52 / 100, NULL);
			LineTo(dc, x + s * 43 / 100, y + s * 72 / 100);
			LineTo(dc, x + s * 78 / 100, y + s * 30 / 100);
			SelectObject(dc, o2); DeleteObject(cp);
		}
		else if (!radio && chk == 2)
		{
			RECT dash = { br.left + box / 4, br.top + box / 2 - 1, br.right - box / 4, br.top + box / 2 + 1 };
			Fill(dc, dash, RGB(255, 255, 255));
		}

		RECT tr = rc; tr.left = br.right + Scale(h, 8);
		SetTextColor(dc, enabled ? g_pal.text : g_pal.dim);
		UINT fl = DT_LEFT | DT_VCENTER | ((style & BS_MULTILINE) ? DT_WORDBREAK : DT_SINGLELINE) | DT_HIDEPREFIX;
		DrawTextW(dc, txt, -1, &tr, fl);
		if (focus && txt[0])
		{
			RECT calc = tr;
			DrawTextW(dc, txt, -1, &calc, fl | DT_CALCRECT);
			int hgt = calc.bottom - calc.top;
			RECT fr = { tr.left - 2, (rc.bottom - hgt) / 2 - 1, calc.right + 2, (rc.bottom + hgt) / 2 + 1 };
			if (fr.right > rc.right) fr.right = rc.right;
			SetTextColor(dc, g_pal.dim);
			DrawFocusRect(dc, &fr);
		}
		SelectObject(dc, oldF);
	}

	LRESULT CALLBACK ButtonProc(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR id, DWORD_PTR ref)
	{
		BtnState* s = (BtnState*)ref;
		switch (m)
		{
		case WM_ERASEBKGND:
			return 1;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC dc = BeginPaint(h, &ps);
			RECT rc; GetClientRect(h, &rc);
			if ((GetWindowLong(h, GWL_STYLE) & BS_TYPEMASK) == BS_GROUPBOX)
				PaintButton(h, dc, s->hot);        // transparent: paint straight onto the dialog
			else
			{ Buffered b(dc, rc); PaintButton(h, b.mem, s->hot); }
			EndPaint(h, &ps);
			return 0;
		}
		case WM_PRINTCLIENT:
			PaintButton(h, (HDC)w, false);
			return 0;
		case WM_MOUSEMOVE:
			if (!s->tracking)
			{
				TRACKMOUSEEVENT t = { sizeof(t), TME_LEAVE, h, 0 };
				TrackMouseEvent(&t);
				s->tracking = true;
			}
			if (!s->hot) { s->hot = true; InvalidateRect(h, NULL, FALSE); }
			break;
		case WM_MOUSELEAVE:
			s->tracking = false; s->hot = false;
			InvalidateRect(h, NULL, FALSE);
			break;
		case WM_NCDESTROY:
			RemoveWindowSubclass(h, ButtonProc, id);
			delete s;
			break;
		case WM_SETFOCUS: case WM_KILLFOCUS: case WM_ENABLE: case WM_SETTEXT: case WM_LBUTTONDOWN:
		case WM_LBUTTONUP: case WM_KEYDOWN: case WM_KEYUP: case BM_SETCHECK: case BM_SETSTATE: case BM_CLICK:
		{
			LRESULT r = DefSubclassProc(h, m, w, l);
			InvalidateRect(h, NULL, FALSE);
			return r;
		}
		}
		return DefSubclassProc(h, m, w, l);
	}

	void SkinButton(HWND h)
	{
		LONG style = GetWindowLong(h, GWL_STYLE);
		UINT type = style & BS_TYPEMASK;
		if (style & (BS_PUSHLIKE | BS_ICON | BS_BITMAP))
		{
			Theme(h, L"DarkMode_Explorer");
			return;
		}
		switch (type)
		{
		case BS_CHECKBOX: case BS_AUTOCHECKBOX: case BS_3STATE: case BS_AUTO3STATE:
		case BS_RADIOBUTTON: case BS_AUTORADIOBUTTON: case BS_GROUPBOX:
			SetWindowTheme(h, L"", L"");
			SetWindowSubclass(h, ButtonProc, ID_BTN, (DWORD_PTR) new BtnState{ false, false });
			break;
		case BS_PUSHBUTTON: case BS_DEFPUSHBUTTON:
			Theme(h, L"DarkMode_Explorer");
			break;
		}
	}

	// ------------------------------------------------------------------ tab control (flat, accent underline)
	LRESULT CALLBACK TabProc(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR id, DWORD_PTR ref)
	{
		int* hotIdx = (int*)ref;
		switch (m)
		{
		case WM_ERASEBKGND:
		{
			RECT rc; GetClientRect(h, &rc);
			Fill((HDC)w, rc, g_pal.bg);
			return 1;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC dc = BeginPaint(h, &ps);
			RECT cl; GetClientRect(h, &cl);
			RECT disp = cl; SendMessage(h, TCM_ADJUSTRECT, FALSE, (LPARAM)&disp);
			RECT strip = cl; strip.bottom = disp.top;
			{
				Buffered b(dc, strip);
				Fill(b.mem, strip, g_pal.bgAlt);
				HFONT of = (HFONT)SelectObject(b.mem, ControlFont(h));
				SetBkMode(b.mem, TRANSPARENT);
				int n = (int)SendMessage(h, TCM_GETITEMCOUNT, 0, 0);
				int cur = (int)SendMessage(h, TCM_GETCURSEL, 0, 0);
				for (int i = 0; i < n; i++)
				{
					RECT r;
					if (!SendMessage(h, TCM_GETITEMRECT, i, (LPARAM)&r)) continue;
					bool sel = (i == cur), hot = (i == *hotIdx);
					RECT t = r; t.top += 2; t.bottom = strip.bottom;
					Fill(b.mem, t, sel ? g_pal.bg : (hot ? g_pal.hot : g_pal.bgAlt));
					if (sel)
					{
						RECT bar = { r.left, strip.bottom - Scale(h, 2) - 1, r.right, strip.bottom - 1 };
						Fill(b.mem, bar, g_pal.accent);
					}
					wchar_t txt[128] = L"";
					TCITEMW it = {}; it.mask = TCIF_TEXT; it.pszText = txt; it.cchTextMax = 128;
					SendMessage(h, TCM_GETITEMW, i, (LPARAM)&it);
					SetTextColor(b.mem, sel ? g_pal.text : g_pal.dim);
					RECT tr = r; tr.bottom = strip.bottom - Scale(h, 2);
					DrawTextW(b.mem, txt, -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_HIDEPREFIX | DT_END_ELLIPSIS);
				}
				RECT line = { strip.left, strip.bottom - 1, strip.right, strip.bottom };
				Fill(b.mem, line, g_pal.border);
				SelectObject(b.mem, of);
			}
			EndPaint(h, &ps);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			TCHITTESTINFO ht = { { GET_X_LPARAM(l), GET_Y_LPARAM(l) }, 0 };
			int i = (int)SendMessage(h, TCM_HITTEST, 0, (LPARAM)&ht);
			if (i != *hotIdx)
			{
				*hotIdx = i; InvalidateRect(h, NULL, FALSE);
				TRACKMOUSEEVENT t = { sizeof(t), TME_LEAVE, h, 0 };
				TrackMouseEvent(&t);
			}
			break;
		}
		case WM_MOUSELEAVE:
			*hotIdx = -1; InvalidateRect(h, NULL, FALSE);
			break;
		case WM_NCDESTROY:
			RemoveWindowSubclass(h, TabProc, id);
			delete hotIdx;
			break;
		}
		return DefSubclassProc(h, m, w, l);
	}

	void SkinTab(HWND h)
	{
		LONG style = GetWindowLong(h, GWL_STYLE);
		if (style & (TCS_VERTICAL | TCS_BOTTOM | TCS_BUTTONS | TCS_OWNERDRAWFIXED))
			return;
		SetWindowSubclass(h, TabProc, ID_TAB, (DWORD_PTR) new int(-1));
	}

	// ------------------------------------------------------------------ status bar
	LRESULT CALLBACK StatusProc(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR id, DWORD_PTR)
	{
		switch (m)
		{
		case WM_ERASEBKGND:
			return 1;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC dc = BeginPaint(h, &ps);
			RECT cl; GetClientRect(h, &cl);
			{
				Buffered b(dc, cl);
				Fill(b.mem, cl, g_pal.bgAlt);
				RECT line = { cl.left, cl.top, cl.right, cl.top + 1 };
				Fill(b.mem, line, g_pal.border);
				HFONT of = (HFONT)SelectObject(b.mem, ControlFont(h));
				SetBkMode(b.mem, TRANSPARENT);
				SetTextColor(b.mem, g_pal.dim);
				int n = (int)SendMessage(h, SB_GETPARTS, 0, 0);
				for (int i = 0; i < n; i++)
				{
					RECT r;
					if (!SendMessage(h, SB_GETRECT, i, (LPARAM)&r)) continue;
					wchar_t txt[512] = L"";
					int len = LOWORD(SendMessage(h, SB_GETTEXTLENGTHW, i, 0));
					if (len > 0 && len < 512) SendMessage(h, SB_GETTEXTW, i, (LPARAM)txt);
					RECT tr = r; tr.left += Scale(h, 8); tr.right -= Scale(h, 6);
					DrawTextW(b.mem, txt, -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
					if (i < n - 1)
					{
						RECT sep = { r.right - 1, r.top + Scale(h, 5), r.right, r.bottom - Scale(h, 4) };
						Fill(b.mem, sep, g_pal.border);
					}
				}
				SelectObject(b.mem, of);
			}
			EndPaint(h, &ps);
			return 0;
		}
		case WM_NCDESTROY:
			RemoveWindowSubclass(h, StatusProc, id);
			break;
		}
		return DefSubclassProc(h, m, w, l);
	}

	// ------------------------------------------------------------------ toolbar background
	LRESULT CALLBACK ToolbarProc(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR id, DWORD_PTR)
	{
		if (m == WM_ERASEBKGND)
		{
			RECT rc; GetClientRect(h, &rc);
			Fill((HDC)w, rc, g_pal.bgAlt);
			return 1;
		}
		if (m == WM_NCPAINT) { PaintNcDark(h, g_pal.bgAlt); return 0; }
		if (m == WM_PAINT)
		{
			RECT u;
			if (GetUpdateRect(h, &u, FALSE))
			{
				HDC dc = GetDC(h);
				Fill(dc, u, g_pal.bgAlt);
				ReleaseDC(h, dc);
			}
		}
		if (m == WM_NCDESTROY)
			RemoveWindowSubclass(h, ToolbarProc, id);
		return DefSubclassProc(h, m, w, l);
	}

	// Flat hover / pressed look for MFC toolbar buttons. The toolbar sends NM_CUSTOMDRAW to the window that
	// was its parent at creation time (the frame), so both the frame and the dock bar route it here.
	bool ToolbarCustomDraw(LPARAM l, LRESULT& res)
	{
		NMHDR* nh = (NMHDR*)l;
			if (nh->code == NM_CUSTOMDRAW && ClassIs(nh->hwndFrom, L"ToolbarWindow32"))
			{
				NMTBCUSTOMDRAW* cd = (NMTBCUSTOMDRAW*)l;
				if (cd->nmcd.dwDrawStage == CDDS_PREERASE)
				{
					RECT cr; GetClientRect(nh->hwndFrom, &cr);
					Fill(cd->nmcd.hdc, cr, g_pal.bgAlt);
					{ res = CDRF_SKIPDEFAULT; return true; }
				}
				if (cd->nmcd.dwDrawStage == CDDS_PREPAINT)
				{
					RECT cr; GetClientRect(nh->hwndFrom, &cr);
					Fill(cd->nmcd.hdc, cr, g_pal.bgAlt);
					{ res = CDRF_NOTIFYITEMDRAW; return true; }
				}
				if (cd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
				{
					UINT st = cd->nmcd.uItemState;
					bool down = (st & CDIS_SELECTED) != 0, hot = (st & CDIS_HOT) != 0, on = (st & CDIS_CHECKED) != 0;
					if (down || hot || on)
					{
						RECT r = cd->nmcd.rc; InflateRect(&r, -1, -1);
						HBRUSH br = CreateSolidBrush(down ? g_pal.border : (on ? g_pal.border : g_pal.hot));
						HPEN pen = CreatePen(PS_SOLID, 1, (on || down) ? g_pal.accent : g_pal.hot);
						HPEN op = (HPEN)SelectObject(cd->nmcd.hdc, pen);
						HBRUSH ob = (HBRUSH)SelectObject(cd->nmcd.hdc, br);
						RoundRect(cd->nmcd.hdc, r.left, r.top, r.right, r.bottom, 8, 8);
						SelectObject(cd->nmcd.hdc, ob); SelectObject(cd->nmcd.hdc, op);
						DeleteObject(br); DeleteObject(pen);
					}
					{ res = TBCDRF_NOBACKGROUND | TBCDRF_NOEDGES | TBCDRF_NOOFFSET; return true; }
				}
			}
		return false;
	}

	// Dock bars host the toolbar: they paint the background and the flat hover/pressed buttons.
	LRESULT CALLBACK DockBarProc(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR id, DWORD_PTR)
	{
		switch (m)
		{
		case WM_ERASEBKGND:
		{
			RECT rc; GetClientRect(h, &rc);
			Fill((HDC)w, rc, g_pal.bgAlt);
			return 1;
		}
		case WM_NCPAINT:
			PaintNcDark(h, g_pal.bgAlt);
			return 0;
		case WM_NOTIFY:
		{
			LRESULT res;
			if (ToolbarCustomDraw(l, res)) return res;
			break;
		}
		case WM_NCDESTROY:
			RemoveWindowSubclass(h, DockBarProc, id);
			break;
		}
		return DefSubclassProc(h, m, w, l);
	}

	// ------------------------------------------------------------------ top level frames: dark caption + dark menu bar
	struct UAHMENU { HMENU hmenu; HDC hdc; DWORD dwFlags; };
	struct UAHMENUITEMMETRICS { DWORD v[8]; };
	struct UAHMENUPOPUPMETRICS { DWORD v[5]; };
	struct UAHMENUITEM { int iPosition; UAHMENUITEMMETRICS umim; UAHMENUPOPUPMETRICS umpm; };
	struct UAHDRAWMENUITEM { DRAWITEMSTRUCT dis; UAHMENU um; UAHMENUITEM umi; };
	const UINT WM_UAHDRAWMENU = 0x0091, WM_UAHDRAWMENUITEM = 0x0092;

	void DrawMenuBottomLine(HWND h)
	{
		MENUBARINFO mbi = { sizeof(mbi) };
		if (!GetMenuBarInfo(h, OBJID_MENU, 0, &mbi)) return;
		RECT cl; GetClientRect(h, &cl);
		MapWindowPoints(h, NULL, (POINT*)&cl, 2);
		RECT wr; GetWindowRect(h, &wr);
		OffsetRect(&cl, -wr.left, -wr.top);
		RECT line = cl; line.bottom = line.top; line.top--;
		HDC dc = GetWindowDC(h);
		Fill(dc, line, g_pal.bg);
		ReleaseDC(h, dc);
	}

	LRESULT CALLBACK FrameProc(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR id, DWORD_PTR)
	{
		switch (m)
		{
		case WM_UAHDRAWMENU:
		{
			UAHMENU* um = (UAHMENU*)l;
			MENUBARINFO mbi = { sizeof(mbi) };
			if (!GetMenuBarInfo(h, OBJID_MENU, 0, &mbi)) break;
			RECT wr; GetWindowRect(h, &wr);
			RECT rc = mbi.rcBar; OffsetRect(&rc, -wr.left, -wr.top);
			Fill(um->hdc, rc, g_pal.bg);
			return TRUE;
		}
		case WM_UAHDRAWMENUITEM:
		{
			UAHDRAWMENUITEM* u = (UAHDRAWMENUITEM*)l;
			wchar_t txt[128] = L"";
			MENUITEMINFOW mii = { sizeof(mii) };
			mii.fMask = MIIM_STRING; mii.dwTypeData = txt; mii.cch = 127;
			GetMenuItemInfoW(u->um.hmenu, u->umi.iPosition, TRUE, &mii);
			DWORD st = u->dis.itemState;
			bool hot = (st & (ODS_HOTLIGHT | ODS_SELECTED)) != 0;
			Fill(u->um.hdc, u->dis.rcItem, hot ? g_pal.hot : g_pal.bg);
			SetBkMode(u->um.hdc, TRANSPARENT);
			SetTextColor(u->um.hdc, (st & (ODS_INACTIVE | ODS_DISABLED | ODS_GRAYED)) ? g_pal.dim : g_pal.text);
			DWORD fl = DT_CENTER | DT_SINGLELINE | DT_VCENTER | ((st & ODS_NOACCEL) ? DT_HIDEPREFIX : 0);
			HFONT of = (HFONT)SelectObject(u->um.hdc, g_uiFont);
			DrawTextW(u->um.hdc, txt, -1, &u->dis.rcItem, fl);
			SelectObject(u->um.hdc, of);
			return TRUE;
		}
		case WM_ERASEBKGND:
		{
			RECT rc; GetClientRect(h, &rc);
			Fill((HDC)w, rc, g_pal.bg);
			return 1;
		}
		case WM_NOTIFY:
		{
			LRESULT res;
			if (ToolbarCustomDraw(l, res)) return res;
			break;
		}
		case WM_NCPAINT:
		case WM_NCACTIVATE:
		{
			LRESULT r = DefSubclassProc(h, m, w, l);
			DrawMenuBottomLine(h);
			return r;
		}
		case WM_SETTINGCHANGE:
			if (l && lstrcmpW((LPCWSTR)l, L"ImmersiveColorSet") == 0) SkinTopLevel(h);
			break;
		case WM_NCDESTROY:
			RemoveWindowSubclass(h, FrameProc, id);
			break;
		}
		return DefSubclassProc(h, m, w, l);
	}

	// ------------------------------------------------------------------ per window dispatch from the hook
	void SkinControl(HWND h)
	{
		wchar_t cls[64] = L"";
		GetClassNameW(h, cls, 64);
		if (!_wcsicmp(cls, L"Button"))                    SkinButton(h);
		else if (!_wcsicmp(cls, L"Edit"))                 Theme(h, L"DarkMode_CFD");
		else if (!_wcsicmp(cls, L"ComboBox"))             Theme(h, L"DarkMode_CFD");
		else if (!_wcsicmp(cls, L"ListBox"))              Theme(h, L"DarkMode_Explorer");
		else if (!_wcsicmp(cls, L"ScrollBar"))            Theme(h, L"DarkMode_Explorer");
		else if (!_wcsicmp(cls, L"msctls_updown32"))      Theme(h, L"DarkMode_Explorer");
		else if (!_wcsicmp(cls, L"msctls_trackbar32"))    Theme(h, L"DarkMode_Explorer");
		else if (!_wcsicmp(cls, L"msctls_progress32"))    Theme(h, L"DarkMode_Explorer");
		else if (!_wcsicmp(cls, L"SysTabControl32"))      SkinTab(h);
		else if (!_wcsicmp(cls, L"msctls_statusbar32"))   SetWindowSubclass(h, StatusProc, ID_SBAR, 0);
		else if (!_wcsnicmp(cls, L"AfxControlBar", 13) || !_wcsnicmp(cls, L"Afx:ControlBar", 14))
			SetWindowSubclass(h, DockBarProc, ID_TBAR + 100, 0);
		else if (!_wcsicmp(cls, L"ToolbarWindow32"))
		{
			SendMessage(h, TB_SETSTYLE, 0, (GetWindowLong(h, GWL_STYLE) | TBSTYLE_CUSTOMERASE) & ~TBSTYLE_TRANSPARENT);
			SetWindowSubclass(h, ToolbarProc, ID_TBAR, 0);
		}
		else if (!_wcsicmp(cls, L"SysTreeView32"))
		{
			Theme(h, L"DarkMode_Explorer");
			TreeView_SetBkColor(h, g_pal.ctrl);
			TreeView_SetTextColor(h, g_pal.text);
			TreeView_SetLineColor(h, g_pal.border);
		}
		else if (!_wcsicmp(cls, L"SysListView32"))
		{
			Theme(h, L"DarkMode_Explorer");
			ListView_SetBkColor(h, g_pal.ctrl);
			ListView_SetTextBkColor(h, g_pal.ctrl);
			ListView_SetTextColor(h, g_pal.text);
			HWND hdr = ListView_GetHeader(h);
			if (hdr) Theme(hdr, L"DarkMode_ItemsView");
		}
	}

	bool IsCommonDialog(HWND h)
	{
		// Only skin dialogs that are MFC windows; system message boxes / common dialogs keep the stock look.
		CWnd* w = CWnd::FromHandlePermanent(h);
		return !w || w->IsKindOf(RUNTIME_CLASS(CCommonDialog));
	}

	LRESULT CALLBACK RetHook(int code, WPARAM wp, LPARAM lp)
	{
		if (code == HC_ACTION)
		{
			const CWPRETSTRUCT* m = (const CWPRETSTRUCT*)lp;
			switch (m->message)
			{
			case WM_CREATE:
			{
				LONG style = GetWindowLong(m->hwnd, GWL_STYLE);
				if (style & WS_CHILD)
					SkinControl(m->hwnd);
				else if ((style & WS_CAPTION) == WS_CAPTION)
					SkinTopLevel(m->hwnd);
				break;
			}
			case WM_INITDIALOG:
				if (!IsCommonDialog(m->hwnd))
				{
					SkinDialogLike(m->hwnd);
					if (!(GetWindowLong(m->hwnd, GWL_STYLE) & WS_CHILD))
						SkinTopLevel(m->hwnd);
				}
				break;
			case WM_CTLCOLORSTATIC: case WM_CTLCOLOREDIT: case WM_CTLCOLORBTN: case WM_CTLCOLORLISTBOX:
				// Dialog bars / property pages that never got WM_INITDIALOG through us.
				if (!IsCommonDialog(m->hwnd) && !ClassIs(m->hwnd, L"ComboBox"))
					SkinDialogLike(m->hwnd);
				break;
			}
		}
		return CallNextHookEx(g_hook, code, wp, lp);
	}
} // anonymous namespace

const Palette& P() { return g_pal; }

void Init()
{
	INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES |
		ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES | ICC_UPDOWN_CLASS | ICC_STANDARD_CLASSES };
	InitCommonControlsEx(&icc);

	HMODULE ux = LoadLibraryW(L"uxtheme.dll");
	if (ux)
	{
		fnSetPreferredAppMode setMode = (fnSetPreferredAppMode)GetProcAddress(ux, MAKEINTRESOURCEA(135));
		fnFlushMenuThemes flush = (fnFlushMenuThemes)GetProcAddress(ux, MAKEINTRESOURCEA(136));
		pAllowDarkModeForWindow = (fnAllowDarkModeForWindow)GetProcAddress(ux, MAKEINTRESOURCEA(133));
		if (setMode) { setMode(PAM_ForceDark); g_darkApi = true; }
		if (flush) flush();
	}
	HMODULE u32 = GetModuleHandleW(L"user32.dll");
	if (u32) pGetDpiForWindow = (fnGetDpiForWindow)GetProcAddress(u32, "GetDpiForWindow");

	g_brBg = CreateSolidBrush(g_pal.bg);
	g_brAlt = CreateSolidBrush(g_pal.bgAlt);
	g_brCtrl = CreateSolidBrush(g_pal.ctrl);

	NONCLIENTMETRICSW ncm = { sizeof(ncm) };
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	wcscpy_s(ncm.lfMessageFont.lfFaceName, L"Segoe UI");
	g_uiFont = CreateFontIndirectW(&ncm.lfMessageFont);

	g_hook = SetWindowsHookExW(WH_CALLWNDPROCRET, RetHook, NULL, GetCurrentThreadId());
}

void Shutdown()
{
	if (g_hook) { UnhookWindowsHookEx(g_hook); g_hook = NULL; }
	if (g_brBg) DeleteObject(g_brBg);
	if (g_brAlt) DeleteObject(g_brAlt);
	if (g_brCtrl) DeleteObject(g_brCtrl);
	if (g_uiFont) DeleteObject(g_uiFont);
	g_brBg = g_brAlt = g_brCtrl = NULL; g_uiFont = NULL;
}

void SkinTopLevel(HWND h)
{
	BOOL dark = TRUE;
	DwmSetWindowAttribute(h, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark, sizeof(dark));
	DwmSetWindowAttribute(h, 19, &dark, sizeof(dark));
	COLORREF cap = g_pal.bg, txt = g_pal.text, brd = g_pal.border;
	DwmSetWindowAttribute(h, 35 /*DWMWA_CAPTION_COLOR*/, &cap, sizeof(cap));
	DwmSetWindowAttribute(h, 36 /*DWMWA_TEXT_COLOR*/, &txt, sizeof(txt));
	DwmSetWindowAttribute(h, 34 /*DWMWA_BORDER_COLOR*/, &brd, sizeof(brd));
	if (pAllowDarkModeForWindow) pAllowDarkModeForWindow(h, TRUE);
	if (!GetWindowSubclass(h, FrameProc, ID_FRAME, NULL))
		SetWindowSubclass(h, FrameProc, ID_FRAME, 0);
	SetWindowPos(h, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}


// ---------------------------------------------------------------------- toolbar icons
void ModernizeToolbar(HWND hTb, UINT resId)
{
	HBITMAP src = (HBITMAP)LoadImageW(AfxGetResourceHandle(), MAKEINTRESOURCEW(resId), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (!src) return;
	BITMAP bm; GetObject(src, sizeof(bm), &bm);
	const int W = bm.bmWidth, H = bm.bmHeight;
	if (W <= 0 || H <= 0 || (W % H) != 0) { DeleteObject(src); return; }
	const int cell = H, count = W / H;

	BITMAPINFO bi = {}; bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = W; bi.bmiHeader.biHeight = -H; bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
	DWORD* px = NULL;
	HBITMAP dib = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&px, NULL, 0);
	HBITMAP dibDis = NULL; DWORD* pxd = NULL;
	if (!dib) { DeleteObject(src); return; }
	HDC sdc = CreateCompatibleDC(NULL);
	GetDIBits(sdc, src, 0, H, px, &bi, DIB_RGB_COLORS);
	DeleteDC(sdc);
	DeleteObject(src);

	// Flood fill the background colour (taken from each cell's top-left pixel) from the cell border.
	for (int c = 0; c < count; c++)
	{
		const int x0 = c * cell;
		const DWORD key = px[x0] & 0x00FFFFFF;
		std::vector<POINT> q;
		std::vector<char> seen(cell * cell, 0);
		for (int i = 0; i < cell; i++)
		{
			POINT a[4] = { {i, 0}, {i, cell - 1}, {0, i}, {cell - 1, i} };
			for (int k = 0; k < 4; k++) q.push_back(a[k]);
		}
		while (!q.empty())
		{
			POINT p = q.back(); q.pop_back();
			if (p.x < 0 || p.y < 0 || p.x >= cell || p.y >= cell) continue;
			if (seen[p.y * cell + p.x]) continue;
			DWORD& d = px[p.y * W + x0 + p.x];
			if ((d & 0x00FFFFFF) != key) continue;
			seen[p.y * cell + p.x] = 1;
			d = 0; // fully transparent
			POINT n[4] = { {p.x + 1, p.y}, {p.x - 1, p.y}, {p.x, p.y + 1}, {p.x, p.y - 1} };
			for (int k = 0; k < 4; k++) q.push_back(n[k]);
		}
	}
	// Opaque pixels: add alpha, lighten near-black outlines so they read on a dark toolbar.
	dibDis = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&pxd, NULL, 0);
	for (int i = 0; i < W * H; i++)
	{
		DWORD d = px[i];
		if (d == 0) { if (pxd) pxd[i] = 0; continue; }
		int b = d & 255, g = (d >> 8) & 255, r = (d >> 16) & 255;
		int lum = (r * 30 + g * 59 + b * 11) / 100;
		if (lum < 70) { r = 214; g = 219; b = 232; }
		px[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
		if (pxd)
		{
			int gray = (r * 30 + g * 59 + b * 11) / 100;
			pxd[i] = (90u << 24) | (gray << 16) | (gray << 8) | gray; // dimmed, semi transparent
		}
	}
	HIMAGELIST il = ImageList_Create(cell, cell, ILC_COLOR32, count, 0);
	ImageList_Add(il, dib, NULL);
	SendMessage(hTb, TB_SETIMAGELIST, 0, (LPARAM)il);
	if (dibDis)
	{
		HIMAGELIST ild = ImageList_Create(cell, cell, ILC_COLOR32, count, 0);
		ImageList_Add(ild, dibDis, NULL);
		SendMessage(hTb, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)ild);
		DeleteObject(dibDis);
	}
	DeleteObject(dib);
	InvalidateRect(hTb, NULL, TRUE);
}

} // namespace ModernUI

// ---------------------------------------------------------------------- flat dark splitter
CModernSplitter::CModernSplitter()
{
	m_cxSplitter = m_cySplitter = 4;
	m_cxSplitterGap = m_cySplitterGap = 4;
	m_cxBorder = m_cyBorder = 1;
	m_cxBorderShare = m_cyBorderShare = 0;
}

void CModernSplitter::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect)
{
	if (!pDC) { CSplitterWnd::OnDrawSplitter(pDC, nType, rect); return; }
	const ModernUI::Palette& p = ModernUI::P();
	switch (nType)
	{
	case splitBorder:
		pDC->Draw3dRect(rect, p.border, p.border);
		return;
	case splitBox: case splitBar: case splitIntersection:
	default:
		pDC->FillSolidRect(rect, p.bg);
		return;
	}
}

namespace ModernUI
{

// ---------------------------------------------------------------------- OpenGL helpers
void LerpColor(const unsigned char a[3], const unsigned char b[3], float t, float out[3])
{
	for (int i = 0; i < 3; i++)
		out[i] = (a[i] + (b[i] - a[i]) * t) / 255.0f;
}

void DrawGradientBackground(const unsigned char bottomRgb[3])
{
	const unsigned char white[3] = { 120, 132, 168 }, black[3] = { 8, 9, 12 };
	float top[3], bot[3];
	LerpColor(bottomRgb, white, 0.22f, top);
	LerpColor(bottomRgb, black, 0.45f, bot);

	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_TRANSFORM_BIT | GL_POLYGON_BIT | GL_LIGHTING_BIT);
	glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE); glDisable(GL_BLEND); glDisable(GL_ALPHA_TEST);
	glDepthMask(GL_FALSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();
	glShadeModel(GL_SMOOTH);
	glBegin(GL_QUADS);
	glColor3fv(bot); glVertex2f(0, 0); glVertex2f(1, 0);
	glColor3fv(top); glVertex2f(1, 1); glVertex2f(0, 1);
	glEnd();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glPopAttrib();
}

} // namespace ModernUI
