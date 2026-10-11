# Blastmaster Suite build and runtime validation

Release metadata: **Version 1.0, Milestone 1, Build 67**.

## 1. Build prerequisites on Windows (MSYS2 UCRT64)

Install the matching UCRT64 toolchain and Qt 6 packages used by the repository. Use the same MSYS2 environment for CMake, Ninja, Qt, and the MinGW compiler; do not mix MSVC-built Qt DLLs with MinGW-built executables.

From the repository root in the **MSYS2 UCRT64 terminal**:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-qt6-tools mingw-w64-ucrt-x86_64-sqlite3
```

Confirm these tools resolve from the same UCRT64 prefix:

```bash
which cmake
which ninja
which qmake6
which windeployqt
which g++
```

## 2. Clean-build both editions

Run these commands from the repository root. Separate build directories prevent Standard and Professional configurations from contaminating each other.

**Standard edition**

```bash
cmake -S . -B build-standard -G Ninja -DBLASTMASTER_BUILD_PROFESSIONAL=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-standard --clean-first
```

**Professional edition**

```bash
cmake -S . -B build-professional -G Ninja -DBLASTMASTER_BUILD_PROFESSIONAL=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build-professional --clean-first
```

If a build directory was previously configured with a different generator, delete that build directory first or choose a new one. Do not reuse a Visual Studio/MSVC build directory with MSYS2 MinGW.

## 3. Expected installed application folders

The installer payload is laid out so each application is installed with its own Qt and compiler runtime beside its executable:

```text
C:\Program Files\Blastmaster Suite\
  BlastmasterSuiteSetup.exe (installer only; normally not copied into the install root)
  Uninstall.exe
  BlastmasterHelp.exe
  Docs\
    blastmaster_docs.exe
    Qt6Core.dll
    Qt6Gui.dll
    Qt6Widgets.dll
    platforms\qwindows.dll
    libwinpthread-1.dll
    libb2-1.dll
  Workbooks\
    blastmaster_workbook.exe
    Qt6Core.dll
    Qt6Gui.dll
    Qt6Widgets.dll
    platforms\qwindows.dll
    libwinpthread-1.dll
    libb2-1.dll
  Presentations\
    blastmaster_presentations.exe
    Qt6Core.dll
    Qt6Gui.dll
    Qt6Widgets.dll
    platforms\qwindows.dll
    libwinpthread-1.dll
    libb2-1.dll
  Databases\ (Professional edition only)
    blastmaster_database.exe
    Qt6Core.dll
    Qt6Gui.dll
    Qt6Widgets.dll
    Qt6Sql.dll
    platforms\qwindows.dll
    sqldrivers\qsqlite.dll
    libwinpthread-1.dll
    libb2-1.dll
  config\activation.ini
```

The exact Qt DLL set depends on which Qt modules the executable uses. Treat the list above as a minimum spot-check, not an exhaustive manifest. `windeployqt` may deploy additional required DLLs and plugins.

## 4. Build/package checks

For each edition:

- Confirm CMake configuration succeeds without missing Qt, compiler, PowerShell, or `windeployqt` errors.
- Confirm all enabled app targets compile and link.
- Confirm the post-build deployment step completes for each app without missing runtime DLL errors.
- For Professional, verify Databases is built and staged; for Standard, verify it is not installed.
- Confirm each app folder contains its executable, the Qt platform plugin under `platforms\`, and its runtime dependencies.
- Install into a clean test directory first. Then test the real Program Files path through the elevated installer.
- Launch every installed application from its Start Menu shortcut while the build tree and MSYS2 directories are not on PATH. This catches accidental dependencies on DLLs outside the install folder.
- Open a sample file by double-clicking each registered file type, then save, close, and reopen it.
- Test uninstall and confirm app folders, Start Menu shortcuts, and Blastmaster-owned registry entries are removed without deleting unrelated user files.

## 5. Runtime troubleshooting

- **Missing `windeployqt`:** add the matching Qt 6 bin directory to PATH and reconfigure CMake.
- **Missing `libwinpthread-1.dll` or `libb2-1.dll`:** install/update the matching MSYS2 UCRT64 runtime packages and ensure those DLLs sit beside the matching `windeployqt` executable. Rebuild the app and installer.
- **Missing `qwindows.dll`:** verify `platforms\qwindows.dll` is in the same application folder as the executable's runtime.
- **Database cannot open SQLite files:** verify `Qt6Sql.dll` and `sqldrivers\qsqlite.dll` are deployed beside the Databases app.
- **App works only from MSYS2:** inspect its imports and runtime folder; do not fix this by copying every DLL into every app. Re-run `windeployqt` for each executable so its Qt module and plugin set is correct.

## Release gate

Do not mark a build as release-ready until both edition builds succeed and the installed applications pass the clean-machine runtime test. Repository changes and a successful compile alone do not prove that the installed package works on another Windows computer.
