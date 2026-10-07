# Blastmaster Suite Help Video Series

This directory defines the official help-video series for the current Blastmaster Suite tree.

## Production style

- **Narration:** Daniel (UK) voice from TTSDEmo / Oddcast.
- **Presenter:** No face, avatar, webcam, or talking-head footage.
- **Visuals:** Direct screen recordings of the current Blastmaster Suite builds, cursor emphasis, menu callouts, simple title cards, and occasional close-ups of dialogs.
- **Aspect ratio:** 16:9.
- **Target resolution:** 1920x1080.
- **Audio:** Voice narration is primary; keep interface sounds subtle.
- **Tone:** Clear, calm, instructional, slightly retro-computing in presentation.
- **Branding:** Blastmaster Suite title card and the relevant application icon.
- **Version scope:** Current `main` branch as of 2026-10-06.

Daniel is an English UK male voice in the underlying TTS voice catalog. Use the Daniel (UK) preset in TTSDEmo rather than substituting another voice.

## Important implementation note

The repository connector can inspect and modify the source repository, but it cannot render or download TTSDEmo audio/video assets directly. Therefore this directory contains the complete production scripts, shot lists, metadata, and naming plan. The final MP4s should be rendered from these scripts with the requested Daniel (UK) narration.

## Video naming

Use:

`BM_HELP_##_<topic>.mp4`

Example:

`BM_HELP_01_SUITE_OVERVIEW.mp4`

## Series

1. Suite overview
2. Installation and product-key setup
3. Docs introduction
4. Docs file management and printing
5. Docs editing and formatting
6. Docs insert, tables, and layout
7. Docs tools, review, windows, and help
8. Workbooks introduction
9. Workbooks editing, formatting, and data tools
10. Presentations introduction
11. Presentations design and slide-show workflow
12. Databases introduction
13. Databases tables and database objects
14. Databases queries, forms, reports, and relationships
15. Setup, installation, Windows integration, and uninstall
16. Standard vs Professional edition and file formats

Each video is designed to stand alone while also working as part of the complete help library.
