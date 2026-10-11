# Blastmaster Presentations Help

**Blastmaster Suite · Presentations · Version 1.0 Milestone 1 Build 67**

Blastmaster Presentations is the suite's slide-authoring application. Its workspace follows the classic mid-1990s presentation model associated with PowerPoint 7.0: a menu bar, dockable Standard, Formatting, and Drawing toolbars, a slide-editing area, and a status bar. This guide uses Blastmaster names and branding; it is an original guide to Blastmaster's implementation, not a copy of Microsoft's original documentation.

## Contents

- [Start a presentation](#start-a-presentation)
- [The workspace](#the-workspace)
- [Create and organize slides](#create-and-organize-slides)
- [Edit slide text](#edit-slide-text)
- [Use the toolbars](#use-the-toolbars)
- [Review and present](#review-and-present)
- [Save, open, and recover work](#save-open-and-recover-work)
- [Print and page setup](#print-and-page-setup)
- [Menu reference](#menu-reference)
- [Troubleshooting](#troubleshooting)

## Start a presentation

1. Choose **File > New** to create a blank presentation.
2. Use **Insert > New Slide** to add a slide.
3. Select a slide in the slide list/workspace before editing its title or body.
4. Choose **File > Save As** the first time you save, and use the native `.prex` format.
5. Continue to save regularly with **File > Save**.

To open a presentation, choose **File > Open**, select a supported presentation file, and confirm. The **File > Recent Presentations** submenu provides a quick route to files opened or saved recently on this computer.

## The workspace

Blastmaster Presentations keeps the classic menu-and-toolbar arrangement rather than using a modern ribbon.

- **Menu bar:** File, Edit, View, Insert, Format, Tools, Slide Show, Window, and Help.
- **Standard toolbar:** common file and editing shortcuts, plus presentation utilities.
- **Formatting toolbar:** text font and size, emphasis, alignment, and bullet controls.
- **Drawing toolbar:** the classic drawing-command group for lines, shapes, text boxes, and colors.
- **Status bar:** feedback about the current command and presentation context.

Toolbars can be moved or floated where the window layout permits. Use **View > Toolbars** to show or hide individual toolbars. A check mark indicates a visible toolbar. If a toolbar is missing, open this submenu and turn it back on.

## Create and organize slides

### Add a slide

Choose **Insert > New Slide**. The new slide becomes part of the current presentation. Add a clear title and keep the body focused on one main idea.

### Duplicate a slide

Choose **Insert > Duplicate Slide** to make a copy of the current slide's title and body as a starting point. Edit the copy so it supports the next point rather than repeating the same information.

### Delete a slide

Choose **Edit > Delete Slide** to remove the current slide. Review the selected slide before confirming your workflow; deletion changes the presentation.

### Navigate and plan

Use the available slide list and presentation workspace to select the slide you want to edit. The classic PowerPoint 7.0 workflow also emphasizes planning the presentation as a sequence: introduction, supporting points, and conclusion. Blastmaster's current implementation provides basic slide operations; full slide-sorter, outline, notes-page, master-slide, transition, and timing workflows are not yet complete.

## Edit slide text

1. Select the slide to edit.
2. Click in the slide's title or body editor.
3. Type or revise the text.
4. Select the text you want to format, then use **Format** commands or the Formatting toolbar.
5. Save the presentation.

The Formatting toolbar supports common text operations such as bold, italic, underline, font selection, font size, alignment, and bullets where the associated editor is available. Use readable wording and consistent formatting. Avoid using color or emphasis as the only way to communicate meaning.

## Use the toolbars

### Standard toolbar

The Standard toolbar presents familiar file, clipboard, undo/redo, spell-check, chart/clip-art, and zoom-style controls. Some entries are currently placeholders and may only report their status instead of performing the full PowerPoint-equivalent operation. Do not rely on a placeholder as proof that a feature is available.

### Formatting toolbar

Use **Font** to choose a typeface, **Size** to set point size, **Bold**, **Italic**, and **Underline** to emphasize selected text, and **Left**, **Center**, or **Right** to change paragraph alignment. **Bullets** applies a bulleted list when the text editor supports it.

### Drawing toolbar

The Drawing toolbar lists common object-creation and appearance commands such as Line, Arrow, Rectangle, Oval, Text Box, Line Color, and Fill Color. The toolbar layout is present for the classic workflow, but not every listed drawing command is fully implemented yet.

### Show or hide toolbars

1. Open **View > Toolbars**.
2. Select a toolbar name to toggle its visibility.
3. Reopen the submenu to check or change the selection.

If changes to toolbar placement do not survive restarting the application, treat layout persistence as not yet available in this release.

## Review and present

Choose **Slide Show > View Show** to start the available full-screen presentation mode. Use **Previous** and **Next** to move through slides and **End Show** to return to editing. Add at least one slide before starting the show.

Before presenting:

- Save your latest changes.
- Review slide order and wording.
- Check titles, spelling, contrast, and text size.
- Confirm that the presentation opens on the computer you plan to use.

The current slide-show implementation is a basic full-screen viewer. PowerPoint 7.0-style transitions, rehearsed timings, hidden slides, show setup, and advanced presenter controls are not yet complete in Blastmaster Presentations.

## Save, open, and recover work

- **File > Save:** saves to the current file when a path has been set.
- **File > Save As:** choose a file name and location for a new copy.
- **File > Open:** opens a supported presentation file.
- **File > Recent Presentations:** reopens a recent file.
- **File > Close / Exit:** close the presentation or application; respond to any save prompt carefully.

Use the native `.prex` format to preserve the data supported by Blastmaster Presentations. If opening or saving fails, keep the original file unchanged, check that the destination folder is writable, and try saving a copy to another location. Do not assume that unsupported PowerPoint formats or embedded objects will round-trip without loss.

## Print and page setup

The menu includes **File > Page Setup**, **Print Setup**, and **Print** for the classic office workflow. Availability depends on the implementation and installed printer support in this build. Before printing an important presentation, use any available preview, verify page orientation and paper size, and print a small test. If a command does not open a dialog or produce output, it is not yet a completed feature.

## Menu reference

| Menu | Purpose |
|---|---|
| **File** | New, Open, Recent Presentations, Save, Save As, Page Setup, Print, Properties, and Exit |
| **Edit** | Undo/Redo, clipboard commands, selection, duplication, deletion, Find, and Replace |
| **View** | Workspace views, slide show, toolbars, rulers, guides, and zoom |
| **Insert** | New/Duplicate Slide and the listed content-insertion commands |
| **Format** | Font, bullets, alignment, line spacing, text direction, and slide appearance |
| **Tools** | Spelling, style checking, AutoContent, options, add-ins, and macro-related commands |
| **Slide Show** | View Show and presentation setup, transition, timing, and hide-slide commands |
| **Window** | Window arrangement commands |
| **Help** | Contents and Index, assistance, and About Blastmaster Presentations |

**Important:** The menu layout intentionally reflects the classic PowerPoint 7.0-era command organization, but a menu label alone does not guarantee that the corresponding command is functional. Commands still being developed should not be used for production-critical work.

## Troubleshooting

**The help window opens but the article is blank**
- Rebuild and reinstall the current application so the bundled help resources are refreshed.
- Check whether other help topics render. If all topics are blank, report a suite-wide help-resource issue.

**A toolbar is not visible**
- Open **View > Toolbars** and enable it.
- If the submenu shows no toolbars, restart the app and report the issue.

**The presentation will not save**
- Check the folder and filename, permissions, and available disk space.
- Use **Save As** to write a separate copy before attempting risky recovery steps.

**A menu item only displays a status message**
- That command is a placeholder in this build. Use the implemented slide, text, save, or basic slide-show workflows instead.

**A file looks different after reopening**
- Keep a backup of the original and a copy of the Blastmaster version. Features or objects not supported by the current native format may not be preserved.

## About this guide

This article is maintained for **Blastmaster Presentations** and follows the classic PowerPoint 7.0-era interaction model while retaining Blastmaster Suite branding. Feature descriptions distinguish available basic workflows from commands whose full implementation remains in progress.
