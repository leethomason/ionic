#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace ionic {

enum class Color : uint8_t {
    gray,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    default = gray
};

enum class ColType {
    kDynamic,       // as wide as needed
    kFixed, 	    // specified width
};

class Ionic {
public:
    static void initConsole();

    bool outerBorder = true;
    bool innerHorizontalDivider = true;
    char borderHChar = '-';
    char borderVChar = '|';
    char borderCornerChar = '+';
    int  maxWidth = -1;  // positive will use that value; <=0 will use terminal width

    struct Column {
        ColType type = ColType::kDynamic;
        int requestedWidth = 0;
    };

    void setColumnFormat(const std::vector<Column>& cols);
    void addRow(const std::vector<std::string>& row);
    void print();

    // --- Utility functions --- 
    // Called automatically. Here as public for convenience.
    static constexpr char kWhitespace[] = " \t\n\r";
    static constexpr char kSpace[] = " \t";
    
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

    static std::vector<std::string> wordWrap(const std::string& text, int width);

    struct Break {
        size_t start = 0;
        size_t end = 0;
        size_t next = 0;
    };
    static Break lineBreak(const std::string& text, size_t start, size_t end, int width);

    // --- Testing ---
    static bool test();

private:
    static constexpr int kMinWidth = 3; // minimum column width (for dynamic columns)

    struct Cell {
		std::string text;
        int desiredWidth = 0;
        int nLines = 0;
	};

    std::string _buf;
    std::vector<Column> _cols;
    std::vector<std::vector<Cell>> _rows;

    int terminalWidth() { return 40; }  // FIXME
    std::vector<int> computeWidths(int w) const;   // returns inner column sizes for the given w (width)
    void printTBBorder(const std::vector<int>& innerColWidth);
};

}  // namespace ionic
