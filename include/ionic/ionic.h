#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <ostream>
#include <optional>

namespace ionic {

enum class Color : uint8_t {
    gray,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,

    white,
    brightRed,
    brightGreen,
    brightYellow,
    brightBlue,
    brightMagenta,
    brightCyan,

    default,
    reset,
};

enum class Alignment {
    left,
    right,
    center,
};

enum class ColType {
    dynamic,       // as wide as needed
    fixed, 	    // specified width
};

struct TableOptions {
    bool outerBorder = true;                    // true to draw the outer border
    bool innerHDivider = true;				    // true to draw horizontal dividers between rows
    char borderHChar = '-';                     // specify characters for the border
    char borderVChar = '|';                     // specify characters for the border
    char borderCornerChar = '+';                // specify characters for the border
    int  maxWidth = -1;                         // positive will use that value; <=0 will use terminal width
    Color tableColor = Color::default;          // color of the table border and dividers
    Color textColor = Color::default;		    // default color of the text - can be overridden for individual cells
    Alignment alignment = Alignment::left;	    // default alignment of the text - can be overridden for individual cells
};

/*
*   1. Construct a Table with TableOptions. (See TableOptions for features that can be set.)
*   2. Optional: Set the column format with setColumnFormat(). You can specify columns to be
*      fixed width or dynamic width. If you don't setColumFormat(), all columns will be dynamic.
*   3. Add text rows with addRow(). The number of columns must match the number of columns in the format.
*   4. Optional: Set the color and alignment of individual cells, rows, columns, or the entire table.
*      Use setCell(), setRow(), setColumn(), and setTable().
*   5. Call format() to get the formatted table as a string, or print() to print it to the console,
*      or use the << operator to print it to an ostream.
*/
class Table {
    friend class IonicTest;
public:
    static bool useColor;
    static void initConsole();

    Table(const TableOptions& options = TableOptions()) : _options(options) {}

    struct Column {
        ColType type = ColType::dynamic;
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

    // -- Constants --
    static constexpr char kWhitespace[] = " \t\n\r";
    static constexpr char kSpace[] = " \t";
    static constexpr char kEllipsis[] = "..";
    static constexpr int kMinWidth = 3;         // minimum column width for dynamic columns

    // Utility functions
    // Query the terminal width.
    static int terminalWidth();

private:
    // Remove CR.
    static void normalizeNL(std::string& s) {
        s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    }
    // Remove trailing spaces.
    static void trimRight(std::string& s) {
		s.erase(s.find_last_not_of(kWhitespace) + 1);
	}
    // Find the number of lines, and the maximum width of the lines.
    static int nLines(const std::string&, int& maxWidth);

    struct Break {
        size_t start = 0;
        size_t end = 0;
        size_t next = 0;
    };

    // Breaks the text into lines of the given width.
    static std::vector<Break> wordWrap(const std::string& text, int width);

    // Breaks a single line - usually called by wordWrap.
    static Break lineBreak(const std::string& text, size_t start, size_t end, int width);

    struct Cell {
		std::string text;
        int desiredWidth = 0;
        int nLines = 0;
        Color color = Color::default;
        Alignment alignment = Alignment::left;
	};

    struct Dye {
        Dye(Color c, std::string& s);
        ~Dye();

        static const char* colorCode(Color c);

      private:
		std::string& _s;
        Color _c;
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
