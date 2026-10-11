# Blastmaster Workbooks Help

Workbooks is the spreadsheet application in Blastmaster Suite.

## Getting Started

Choose **File > New** to create a workbook.

Choose **File > Open** to open an existing `.wkbx` workbook.

Choose **File > Save** to save your changes.

Choose **File > Save As** to create another copy or choose another location.

## Working With Cells

A worksheet is organized into rows and columns. Select a cell to enter or edit its contents.

Use the editing commands to cut, copy, paste, undo, redo, find, replace, and move to a particular cell.

## Formatting

Use formatting commands to control the appearance of:

- Cells
- Rows
- Columns
- Numbers
- Text
- Alignment
- Borders

## Calculations

Enter formulas into worksheet cells to calculate values from other cells.

When a workbook contains formulas, verify that the displayed results update correctly after changing source values.

## Data

Use the available data commands to organize worksheet information.

Typical operations include sorting and filtering where implemented by the current build.

## Views

Use View commands to change how the worksheet is displayed.

## Printing

Use page setup and print commands to prepare a worksheet for paper or PDF output where supported.

## Troubleshooting

If a formula does not calculate as expected, check cell references and the formula syntax.

If data appears incorrectly, check the cell formatting and column width.

Save before making large changes so that the previous saved version remains available.

## Classic Workbooks workflow

### Workbook and worksheet basics
Create or open a `.wkbx` workbook, select a cell, and enter text, numbers, dates, or a formula. Save before bulk edits, sorting, or filtering. Use Save As when you need a separate working copy.

### Entering formulas
A formula uses cell references and an expression to calculate a result. After changing an input cell, check that dependent results update. If a formula is shown as text or returns an unexpected result, check the formula prefix, cell references, and the cell's number format. Do not assume every Excel-era function is implemented.

### Sorting and filtering data
Before sorting, select the intended data range and ensure each column has a clear heading. Confirm that whole rows stay together after sorting. Apply a filter only after checking the header row and verify that hidden rows are not mistaken for deleted data. Save a copy before reorganizing important data.

### Formatting and print layout
Use number formats for dates and numeric values, and adjust column width if values appear truncated. In Print Preview, check scaling, orientation, and page boundaries before printing.

### Troubleshooting
- **A result does not update:** re-enter or inspect the formula and confirm referenced cells contain the expected values.
- **Numbers look wrong:** inspect the number format before changing the underlying value.
- **Rows appear missing:** clear filters and inspect the complete range.
- **The workbook will not reopen:** keep a backup and report the exact error message.

Menu labels document the intended workflow; feature availability depends on the current build and should be verified before relying on it.
