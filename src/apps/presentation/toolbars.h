#ifndef PRESENTATION_TOOLBARS_H
#define PRESENTATION_TOOLBARS_H

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace presentation {

/**
 * @brief Toolbar Button structure
 * Represents a single button in a toolbar with icon, tooltip, and action
 */
struct ToolbarButton {
    std::string name;
    std::string tooltip;
    std::string iconPath;
    std::function<void()> action;
    bool enabled = true;
    bool toggleable = false;
    bool toggled = false;
};

/**
 * @brief Toolbar Separator - represents a visual separator between button groups
 */
struct ToolbarSeparator {
    static constexpr const char* TYPE = "separator";
};

/**
 * @brief Standard Toolbar - common file and editing operations
 * Includes: New, Open, Save, Print, Spelling, Cut, Copy, Paste, Format Painter,
 * Undo, Redo, Insert Excel Worksheet, Insert Chart, Insert Clip Art, Zoom
 */
class StandardToolbar {
public:
    StandardToolbar();
    
    std::vector<ToolbarButton> getButtons() const { return buttons; }
    bool isDockable() const { return true; }
    std::string getDefaultPosition() const { return "top"; }
    
private:
    std::vector<ToolbarButton> buttons;
    
    void initializeButtons();
    void onNew();
    void onOpen();
    void onSave();
    void onPrint();
    void onSpelling();
    void onCut();
    void onCopy();
    void onPaste();
    void onFormatPainter();
    void onUndo();
    void onRedo();
    void onInsertExcelWorksheet();
    void onInsertChart();
    void onInsertClipArt();
    void onZoom();
};

/**
 * @brief Formatting Toolbar - text and paragraph formatting
 * Includes: Font, Font Size, Bold, Italic, Underline, Shadow, Alignment,
 * Bullet On/Off, Promote, Demote, Font Color
 */
class FormattingToolbar {
public:
    FormattingToolbar();
    
    std::vector<ToolbarButton> getButtons() const { return buttons; }
    std::vector<std::string> getFontList() const { return fontList; }
    std::vector<uint32_t> getFontSizes() const { return fontSizes; }
    bool isDockable() const { return true; }
    std::string getDefaultPosition() const { return "top"; }
    
private:
    std::vector<ToolbarButton> buttons;
    std::vector<std::string> fontList;
    std::vector<uint32_t> fontSizes;
    
    void initializeButtons();
    void initializeFonts();
    void initializeFontSizes();
    
    void onFontSelected(const std::string& fontName);
    void onFontSizeSelected(uint32_t size);
    void onBold();
    void onItalic();
    void onUnderline();
    void onShadow();
    void onAlignLeft();
    void onAlignCenter();
    void onAlignRight();
    void onBulletToggle();
    void onPromote();
    void onDemote();
    void onFontColor();
};

/**
 * @brief Drawing Toolbar - shape and object drawing tools
 * Includes: Select Objects, Line, Arrow, Rectangle, Oval, Text Box,
 * Freeform, Filled Shapes, Rotate, Line Color, Fill Color, Arrowhead
 * Usually docked at the bottom
 */
class DrawingToolbar {
public:
    DrawingToolbar();
    
    std::vector<ToolbarButton> getButtons() const { return buttons; }
    bool isDockable() const { return true; }
    std::string getDefaultPosition() const { return "bottom"; }
    
private:
    std::vector<ToolbarButton> buttons;
    
    void initializeButtons();
    void onSelectObjects();
    void onLine();
    void onArrow();
    void onRectangle();
    void onOval();
    void onTextBox();
    void onFreeform();
    void onFilledShapes();
    void onRotate();
    void onLineColor();
    void onFillColor();
    void onArrowhead();
};

/**
 * @brief Toolbar Position enumeration
 */
enum class ToolbarPosition {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    FLOATING
};

/**
 * @brief Toolbar Manager - manages all toolbars and their docking state
 */
class ToolbarManager {
public:
    ToolbarManager();
    
    StandardToolbar& getStandardToolbar() { return standardToolbar; }
    FormattingToolbar& getFormattingToolbar() { return formattingToolbar; }
    DrawingToolbar& getDrawingToolbar() { return drawingToolbar; }
    
    void setToolbarPosition(const std::string& toolbarName, ToolbarPosition position);
    ToolbarPosition getToolbarPosition(const std::string& toolbarName) const;
    
    void toggleToolbar(const std::string& toolbarName);
    bool isToolbarVisible(const std::string& toolbarName) const;
    
    void resetToDefaults();
    
private:
    StandardToolbar standardToolbar;
    FormattingToolbar formattingToolbar;
    DrawingToolbar drawingToolbar;
    
    std::vector<std::pair<std::string, ToolbarPosition>> toolbarPositions;
    std::vector<std::pair<std::string, bool>> toolbarVisibility;
    
    void initializeDefaults();
};

/**
 * @brief Shape Types for Drawing Toolbar
 */
enum class ShapeType {
    LINE,
    ARROW,
    RECTANGLE,
    OVAL,
    FREEFORM,
    FILLED_RECTANGLE,
    FILLED_OVAL,
    POLYGON
};

/**
 * @brief Drawing State - manages active drawing tool and properties
 */
class DrawingState {
public:
    DrawingState();
    
    void setActiveShape(ShapeType shape);
    ShapeType getActiveShape() const { return activeShape; }
    
    void setLineColor(uint32_t color);
    uint32_t getLineColor() const { return lineColor; }
    
    void setFillColor(uint32_t color);
    uint32_t getFillColor() const { return fillColor; }
    
    void setLineWidth(uint32_t width);
    uint32_t getLineWidth() const { return lineWidth; }
    
    void setArrowheadStyle(const std::string& style);
    std::string getArrowheadStyle() const { return arrowheadStyle; }
    
private:
    ShapeType activeShape = ShapeType::LINE;
    uint32_t lineColor = 0x000000;      // Black
    uint32_t fillColor = 0xFFFFFF;      // White
    uint32_t lineWidth = 1;
    std::string arrowheadStyle = "none";
};

} // namespace presentation

#endif // PRESENTATION_TOOLBARS_H
