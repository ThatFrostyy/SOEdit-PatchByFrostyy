// Many original message strings in this code base are Russian text stored as Windows-1251 bytes.
// On a non-Russian system the ANSI MessageBox shows them as mojibake. These shims convert such
// strings to UTF-16 before showing them, and leave pure ASCII strings untouched.
#pragma once

inline bool Cp1251HasHighBit(const char* s)
{
	for (; s && *s; ++s)
		if ((unsigned char)*s >= 0x80) return true;
	return false;
}

inline CStringW Cp1251ToWide(const char* s)
{
	CStringW w;
	if (!s || !*s) return w;
	int n = MultiByteToWideChar(1251, 0, s, -1, NULL, 0);
	if (n > 0)
	{
		MultiByteToWideChar(1251, 0, s, -1, w.GetBuffer(n), n);
		w.ReleaseBuffer();
	}
	return w;
}

inline int Cp1251MessageBox(HWND h, LPCSTR text, LPCSTR caption, UINT type)
{
	if (!Cp1251HasHighBit(text) && !Cp1251HasHighBit(caption))
		return ::MessageBoxA(h, text, caption, type);
	return ::MessageBoxW(h, Cp1251ToWide(text), Cp1251ToWide(caption), type);
}

inline int Cp1251MessageBox(LPCSTR text, LPCSTR caption = NULL, UINT type = MB_OK)
{
	CWnd* main = AfxGetMainWnd();
	return Cp1251MessageBox(main ? main->m_hWnd : NULL, text, caption, type);
}

inline int Cp1251AfxMessageBox(LPCSTR text, UINT type = MB_OK, UINT /*helpId*/ = 0)
{
	return Cp1251MessageBox(text, "SOEdit", type);
}

#undef MessageBox
#undef MessageBoxA
#define MessageBoxA Cp1251MessageBox
#define MessageBox  Cp1251MessageBox
#define AfxMessageBox Cp1251AfxMessageBox
