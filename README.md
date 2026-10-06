# Blastmaster Office Suite

A professional C++ office suite with two editions: Professional and Standard. Features include a word processor, presentation program, spreadsheet program (all three in both editions), and a database program (Professional edition only).

## File Extensions

### Docs:
  .dccx : Regular Docs document

### Presentations: 
  .pres: Presentations slideshow

### Workbooks:
  not yet implemented btw

## Features

### Standard Edition
- **Word Processor** - Document editing and formatting
- **Presentation Program** - Slide creation and presentation tools
- **Spreadsheet Program** - Data analysis and calculations

### Professional Edition
- All Standard Edition features
- **Database Program** - Advanced data management and queries

## Installation

### Requirements
- CMake 3.16+
- C++17 compatible compiler
- Qt 6.0+ (for UI)
- SQLite3 (for database functionality)

### Build Instructions
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Product Key Validation
The suite includes built-in product key recognition during setup. Keys are validated against edition type (Standard/Professional) and activation status.

## License
License information here.
