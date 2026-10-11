# Office 95 Feature-Parity Work Plan

## Objective

Bring Blastmaster Docs, Presentations, Workbooks, and Databases to a consistent Windows 95-era desktop interaction model and implement the corresponding user-visible commands. This is a feature-parity target, not a claim to reproduce Microsoft's proprietary source code or every undocumented behavior.

A feature is **complete** only when its menu/toolbar action works, has sensible keyboard accessibility, handles cancellation and errors, preserves data, and is covered by a repeatable test. A status-bar message saying a feature is not implemented does not count as completion.

## Shared desktop behavior

- [ ] Classic gray, square-corner controls, Windows-style menu bar, compact icons, and beveled buttons.
- [ ] Dockable Standard and Formatting toolbars; View > Toolbars toggles each toolbar.
- [ ] Standard File/Edit/Window/Help conventions and accelerator keys.
- [ ] Recent files, correct window title, modified-state indicator, and save-before-discard prompts.
- [ ] Native open/save dialogs with the suite's supported extensions and clear error reporting.
- [ ] Undo/redo and clipboard commands connected to the active editor/selection.
- [ ] Working Print, Print Preview, and Page Setup flows wherever the application supports printable content.
- [ ] Keyboard-only navigation, accessible names, tooltips/status tips, and predictable focus.
- [ ] No dead menu entries: implement the command, disable it with an explanatory tooltip while unsupported, or remove it from the release UI.
- [ ] Test each feature on the supported Windows/Qt/MinGW build and in the packaged installer payload.

## Blastmaster Docs — Word 95-style target

- [ ] New/Open/Close/Save/Save As, recent documents, properties, and dirty-document prompts.
- [ ] Rich-text editing, selection, clipboard, undo/redo, find/replace, and go-to.
- [ ] Character formatting: font, size, bold, italic, underline, color, and highlighting.
- [ ] Paragraph formatting: alignment, indents, tabs, spacing, bullets, and numbering.
- [ ] Page layout: margins, orientation, paper size, headers/footers, page numbers, page breaks, and print preview.
- [ ] Tables: insert table, add/remove rows and columns, cell selection, and table formatting.
- [ ] Insert date/time, symbols, images, hyperlinks, fields, and comments where supported by the document model.
- [ ] Word count and spelling/grammar tools with an explicit language/dictionary strategy.
- [ ] Print and export paths that preserve formatting; verify save/reopen round trips.

## Blastmaster Presentations — PowerPoint 95-style target

- [ ] New/Open/Save/Save As, recent presentations, dirty-state prompts, and slide count/status.
- [ ] Add, duplicate, delete, rename, reorder, and navigate slides.
- [ ] Title/body layouts, slide master/theme baseline, and consistent slide dimensions.
- [ ] Insert and format text, shapes, images, and other supported slide objects.
- [ ] Object selection, alignment, ordering, grouping, and resize operations where supported.
- [ ] Transitions and timings with preview; full-screen slide show with next/previous/escape navigation.
- [ ] Notes/outline/slide-sorter views where the document model supports them.
- [ ] Print preview and handout/slide printing; verify presentation round trips.

## Blastmaster Workbooks — Excel 95-style target

- [ ] New/Open/Save/Save As, recent workbooks, sheet rename/add/delete/reorder, and dirty-state prompts.
- [ ] Grid navigation, range selection, fill/copy/paste, undo/redo, row/column sizing, and freeze panes.
- [ ] Formula bar, formula evaluation, cell references, common functions, and clear formula-error reporting.
- [ ] Number/date formats, font, alignment, borders, fill colors, and cell protection where supported.
- [ ] Sort/filter, find/replace, AutoSum, and basic data validation.
- [ ] Charts with editable source data and labels.
- [ ] Page setup, print area, headers/footers, print preview, and printing.
- [ ] Reopen/save tests covering formulas, formatting, multiple sheets, and data.

## Blastmaster Databases — Access 95-style target

- [ ] New/Open/Save/Save As, recent databases, dirty-state prompts, and database properties.
- [ ] Table create/rename/delete, field names/types, primary keys, and editable datasheets.
- [ ] Record add/edit/delete with validation and transaction/error handling.
- [ ] Query designer and execution for supported select/filter/sort/join queries.
- [ ] Forms and reports with usable design/run workflows, not just menu placeholders.
- [ ] Relationships with key constraints and a visible relationship editor.
- [ ] Sorting/filtering, find/replace, import/export, and supported print/report output.
- [ ] Compact/repair and database conversion only where the underlying SQLite/data model can perform a safe, well-defined equivalent.
- [ ] Backups before destructive maintenance operations; test with copy databases first.

## Acceptance gates

1. **Inventory:** every visible menu and toolbar action is classified as implemented, disabled, or removed.
2. **Functional:** automated tests cover data/document models; GUI smoke tests exercise the main workflows.
3. **Round-trip:** save, close, reopen, and compare content/formatting for each native format.
4. **Failure paths:** cancel dialogs, invalid paths, read-only files, malformed files, and unavailable printers are handled without data loss.
5. **Package:** clean-machine test from the produced setup/ISO, including Qt plugins and compiler/runtime DLLs.
6. **Release:** do not label the suite Office 95 feature-complete until all required acceptance gates pass.

## Implementation order

1. Remove/disable misleading placeholder commands and complete the shared file/edit/toolbar behavior.
2. Complete Docs editing and printing.
3. Complete Workbooks cell/formula/data operations and printing.
4. Complete Presentations slide/object operations and slide show.
5. Complete Databases tables/queries/relationships/forms/reports and maintenance.
6. Run regression tests and packaged-build validation for both editions.
