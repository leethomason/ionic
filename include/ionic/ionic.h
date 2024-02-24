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
    kFixed, 	    // set, fixed width
};

class Ionic {
public:
    static void initConsole();
    static std::vector<std::string_view> wordWrap(const std::string& text, int width);

    bool outerBorder = true;
    bool innerHorizontalDivider = true;
    char borderHChar = '-';
    char borderVChar = '|';
    char borderCornerChar = '+';
    int maxWidth = -1;  // positive will use that value; <0 will use terminal width

    struct Column {
        ColType type = ColType::kDynamic;
        int requestedWidth = 0;
    };

    void setColumnFormat(const std::vector<Column>& cols);
    void addRow(const std::vector<std::string>& row);
    void print();

private:
    std::string _buf;
    std::vector<Column> _cols;
    std::vector<std::vector<std::string>> _rows;

    int terminalWidth() { return 40; }  // FIXME
    void computeWidths(std::vector<int>& innerColWidth);
    void printTBBorder(const std::vector<int>& innerColWidth);
};

}  // namespace ionic
