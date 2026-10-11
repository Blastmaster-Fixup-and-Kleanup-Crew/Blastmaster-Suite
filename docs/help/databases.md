# Blastmaster Databases Help

Databases is the database application included with the Professional edition.

Native file extension: `.dbbx`.

## Database Window

The Database Window is the central workspace for database objects.

The Database toolbar provides quick access to database operations and wizards.

## Tables

Tables store structured information in fields and records.

Use table design to define fields and their properties.

## Queries

Queries retrieve and work with selected database information.

The Query Wizard provides guided query creation.

## Forms

Forms provide an interface for viewing and entering records.

The Form Wizard provides guided form creation.

## Reports

Reports organize database information for viewing and printing.

The Report Wizard provides guided report creation.

## Relationships

Relationships connect related database structures.

Use the Relationships command to inspect and configure relationships supported by the current build.

## Design and Grid Views

Use Design View when configuring the structure of a database object.

Use Grid view when working directly with tabular information.

## Printing

Use Print Preview to inspect database output.

Use Print to produce the final output.

## Saving

Save database changes regularly.

Use the native `.dbbx` format for Blastmaster database files.

## Classic database workflow

### Create or open a database
Use **File > New** to create a database or **File > Open** to open an existing file. Save Blastmaster database files with the native `.dbbx` extension. Keep a backup before editing important records or database structure.

### Tables, fields, and records
Tables store records in rows and fields in columns. Give fields meaningful names and choose suitable data types. Before editing a table, confirm the selected table in the database workspace. Validate changes with a small sample before entering a large amount of data.

### Queries, forms, and reports
Queries are intended to select or organize data; forms provide a record-entry view; reports arrange information for reading or printing. Wizard and design commands should only be relied on when they are implemented in the current build. If an object cannot be created or opened, preserve the database file and note the command and error.

### Relationships and data integrity
Related tables should use consistent key fields. Back up the database before changing relationships or performing destructive operations. Verify that the intended records remain present after each structural change.

### Export and print
Where available, export the selected table to HTML for a portable view. Use Print Preview before printing and verify the selected table and columns.

### Troubleshooting
- **A table is empty:** verify the selected table and clear any active filters.
- **A query returns unexpected rows:** check field names, conditions, and sort settings.
- **A save fails:** verify that the destination is writable and the database is not locked by another process.
- **A structural change goes wrong:** stop editing and restore from a backup rather than experimenting on the only copy.

This guide describes the intended classic workflow; it does not imply that every Access-era object type or wizard is implemented by Blastmaster Databases yet.
