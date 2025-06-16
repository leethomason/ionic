#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <ostream>
#include <optional>
#include <algorithm>

namespace ionic {

// These are mapped to the standard colors, in standard color order...where that makes sense.
// On some terminals, there are 4 gray values:
//    black
//    gray
//    brightGray     <-- where this is the "normal" gray value, which will result from "reset"
//    white
// On others, the "reset" is a different intensity that any of darkGray, gray, or white.
// 16 simple colors shouldn't be this complex.
//
enum class Color : uint8_t {
    black,

    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,

    gray,
    brightGray,

    brightRed,
    brightGreen,
    brightYellow,
    brightBlue,
    brightMagenta,
    brightCyan,

    white,

	kDefault, // same as reset - the "default" color which
    reset,
};

// Returns the terminal code for the given color. 
// This is a standalone utility function, so it doesn't use any Table flags.
std::string colorToStr(Color color);

// Converts a string to a Color.
// Attemptys to be file / user / text friendly.
//   BRIGHT_RED, paleRed -> Color::brightRed
//   red, dark red -> Color::red 
//   gray, grey -> Color::gray
// Note that "dark while" or "bright black" isn't parsed. (As it is 
// gibberish, despite the spec.) Recognizes: "black", "white", "grey",
// where "grey" can be modified by "bright", "pale", or "light". The 
// default grey is "darkGrey" as all the color default to the dark
// variant.
Color strToColor(const std::string& str);

enum class Alignment {
    left,
    right,
    center,
};

enum class ColType {
    flex,       // as wide as needed
    fixed, 	    // specified width
};

struct TableOptions {
    bool outerBorder = true;                    // true to draw the outer border
    bool innerHDivider = true;				    // true to draw horizontal dividers between rows
    bool innerVDivider = true;				    // true to draw vertical dividers between columns

    char borderHChar = '-';                     // specify characters for the border
    char borderVChar = '|';                     // specify characters for the border
    char borderCornerChar = '+';                // specify characters for the border
    
    int  maxWidth = -1;                         // positive will use that value; <=0 will use console width
    int  indent = 0;                            // number of spaces to indent the table (reduces width)
    
    Color tableColor = Color::kDefault;         // color of the table border and dividers
    Color textColor = Color::kDefault;		    // default color of the text - can be overridden for individual cells
    Alignment alignment = Alignment::left;	    // default alignment of the text - can be overridden for individual cells
};

/*
*   1. Construct a Table with TableOptions. (See TableOptions for features that can be set.)
*   2. Optional: Set the column format with setColumnFormat(). You can specify columns to be
*      fixed width or flex width. If you don't setColumFormat(), all columns will be flex.
*   3. Add text rows with addRow(). The number of columns must match the number of columns in the format.
*   4. Optional: Set the color and alignment of individual cells, rows, columns, or the entire table.
*      Use setCell(), setRow(), setColumn(), and setTable().
*   5. Call format() to get the formatted table as a string, or print() to print it to the console,
*      or use the << operator to print it to an ostream.
*/
class Table {
    friend class IonicTest;
public:
    static bool colorEnabled;

    Table(const TableOptions& options = TableOptions()) : _options(options) {}

    struct Column {
        ColType type = ColType::flex;
        int requestedWidth = 0;
    };
    void setColumnFormat(const std::vector<Column>& cols);
    void addRow(const std::vector<std::string>& row);

    void setCell(int row, int col, std::optional<Color>, std::optional<Alignment>);
    void setRow(int row, std::optional<Color>, std::optional<Alignment>);
    void setColumn(int col, std::optional<Color>, std::optional<Alignment>);
    void setTable(std::optional<Color>, std::optional<Alignment>);

    std::string format() const;
    void print() const;

    friend std::ostream& operator<<(std::ostream& os, const Table& t) {
        os << t.format();
        return os;
    }

    // -- Query -- //
    int nRows() const { return static_cast<int>(_rows.size()); }
    int nCols() const { return static_cast<int>(_cols.size()); }

    // -- Constants --
    static constexpr char kWhitespace[] = " \t\n\r";
    static constexpr char kSpace[] = " \t";
    static constexpr char kEllipsis[] = "..";
    static constexpr int kMinWidth = 3;         // minimum column width for flex columns

    // Utility functions
    // Query the terminal width.
    static int consoleWidth();

    // Returns a string wrapped with the given color. The color is reset at the end.
    // This does check the colorEnabled flag.
    static std::string colorize(Color c, const std::string& s);

    struct Break {
        size_t start = 0;   // start of the line
        size_t end = 0;     // end of the line (exclusive)
        size_t next = 0;    // internal use
    };

    // Breaks the text into lines of the given width.
    // NOTE: Ionic is very low-ascii English. (Which sholud be fixed.)
    // But this call (and all of them) assume a 1-char is 1-glyph relationship.
    // params:
    // text: the input string
    // width: width to break on, or 0 to query console
    static std::vector<Break> wordWrap(const std::string& text, int width);


private:
    static void initConsole();

    // Remove CR.
    static void normalizeNL(std::string& s) {
        s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    }
    // Remove trailing spaces.
    static void trimRight(std::string& s) {
        // The npos behavior is weird. If the string is all whitespace, it returns npos. npos+1 is 0,
        // which then returns nothing (which is correct.)
		s.erase(s.find_last_not_of(kWhitespace) + 1);
	}
    // Find the number of lines, and the maximum width of the lines.
    static int nLines(const std::string&, int& maxWidth);

    // Breaks a single line - usually called by wordWrap.
    static Break lineBreak(const std::string& text, size_t start, size_t end, int width);

    struct Cell {
		std::string text;
        int desiredWidth = 0;
        int nLines = 0;
        Color color = Color::kDefault;
        Alignment alignment = Alignment::left;
	};

    struct Dye {
        Dye(Color c, std::string& s);
        ~Dye();

        static const char* colorCode(Color c);

      private:
        Color _c;
		std::string& _s;
    };

    TableOptions _options;
    std::vector<Column> _cols;
    std::vector<std::vector<Cell>> _rows;

    std::vector<int> computeWidths(const int width) const;   // returns inner column sizes for the given width

    void printHorizontalBorder(std::string& s, const std::vector<int>& innerColWidth, bool outer) const;
    void printLeft(std::string& s) const;
    void printCenter(std::string& s) const;
    void printRight(std::string& s) const;
};

}  // namespace ionic
