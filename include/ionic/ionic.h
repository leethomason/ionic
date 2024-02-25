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
    static std::vector<std::string> wordWrap(const std::string& text, int width);

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

    // Utility functions. Called automatically. Here as public for convenience.
    // Remove CR.
    static void normalizeNL(std::string& s) {
        s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    }
    static void trimRight(std::string& s) {
		s.erase(s.find_last_not_of(" \t\n\r") + 1);
	}
    static int nSubStrings(const std::string&, int& maxWidth);

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
