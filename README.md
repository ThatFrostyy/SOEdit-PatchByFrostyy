# SOEdit – Patch by Frostyy

Model, animation and volume editor for *Soldiers: Heroes of World War II* / *Call to Arms* style assets.
This branch gives the classic MFC editor a complete modern make-over and fixes several long-standing bugs.

![SOEdit modern dark UI](docs/screenshot.png)

## What's new

**Look & feel**
- Full dark theme: dark title bar, dark menu bar and popup menus, dark dialogs, tree views, tab strip,
  status bar and docked panels (Windows 10 1809+/Windows 11; degrades gracefully elsewhere).
- Custom-drawn modern check boxes, radio buttons, group boxes and flat tabs with an accent underline.
- Toolbar icons re-rendered with real transparency and light outlines, with flat hover / pressed states.
- Segoe UI everywhere, Common Controls v6 visual styles, Per-Monitor-V2 High-DPI awareness.
- Flat dark splitter bars between the viewports.

**Viewports**
- New dark default colour palette for the 2D and 3D views (all still editable under *Display ▸ Colors*).
- 3D view gets a smooth sky gradient background and a 24-bit depth buffer (less z-fighting).
- Cleaner view captions.

**Fixes**
- Opening a file from the command line (or by drag-and-drop onto the exe) crashed at start-up – fixed.
- Russian (Windows-1251) message boxes showed as mojibake on non-Russian systems – now converted correctly.
- *Display ▸ Colors*: the colour preview was always empty and leaked a device context on every repaint – fixed.
- Settings are now stored under `HKCU\Software\SOEdit\...` instead of the AppWizard placeholder key.
- Build no longer spams thousands of CRT deprecation warnings; multi-processor compilation is on.

## Building

Requirements: Visual Studio 2022 or newer with **Desktop development with C++** and
**C++ MFC for latest build tools** (x86/x64). FreeImage 3.18 (32-bit) is bundled in
`third_party/FreeImage`.

```bash
msbuild SOEdit.vcxproj /p:Configuration=Release /p:Platform=Win32 /m
```

`Release\SOEdit.exe` and `FreeImage.dll` are copied to `Dist\` after a build.

## Code map for the new pieces

| File | Purpose |
|------|---------|
| `ModernUI.h/.cpp` | Dark theme hook, owner-drawn controls, toolbar modernizer, splitter, GL gradient helper |
| `Cp1251Msg.h` | Message box shims that decode Windows-1251 strings |
| `SOEdit.manifest` | DPI awareness, long paths, supported-OS declarations |

## Credits

Original SOEdit by Brian A. Kay, patch v0.7m4 by TonaTor, patch and UI make-over by Frostyy.
FreeImage is used under the FreeImage Public License.
