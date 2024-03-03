
#include "ionic/ionic.h"

#include <iostream>
#include <assert.h>

void PrintRuler(int w)
{
    for (int i = 0; i < w; i++) {
        std::cout << i % 10;
	}
    std::cout << std::endl;
}

void AddVar4Rows(ionic::Table& table)
{
    table.setColumnFormat({ {ionic::ColType::fixed, 2}, {ionic::ColType::flex}, {ionic::ColType::flex} });
    table.addRow({ "0", "A", "The Outer World" });
    table.addRow({ "1", "Hello", "And Another" });
    table.addRow({ "2", "World", "Farther Out" });
}

void PrintDiffs(const ionic::TableOptions& options)
{
    ionic::TableOptions def;
    if (options.outerBorder != def.outerBorder)
		std::cout << "outerBorder: " << options.outerBorder << std::endl;
    if (options.innerHDivider != def.innerHDivider)
        std::cout << "innerHDivider: " << options.innerHDivider << std::endl;
    if (options.innerVDivider != def.innerVDivider)
        std::cout << "innerVDivider: " << options.innerVDivider << std::endl;
    if (options.borderHChar != def.borderHChar)
        std::cout << "borderHChar: " << options.borderHChar << std::endl;
    if (options.borderVChar != def.borderVChar)
        std::cout << "borderVChar: " << options.borderVChar << std::endl;
    if (options.borderCornerChar != def.borderCornerChar)
        std::cout << "borderCornerChar: " << options.borderCornerChar << std::endl;
    if (options.maxWidth != def.maxWidth)
        std::cout << "maxWidth: " << options.maxWidth << std::endl;
    if (options.tableColor != def.tableColor)
        std::cout << "tableColor: " << static_cast<int>(options.tableColor) << std::endl;
    if (options.textColor != def.textColor)
        std::cout << "textColor: " << static_cast<int>(options.textColor) << std::endl;
    if (options.alignment != def.alignment)
        std::cout << "alignment: " << static_cast<int>(options.alignment) << std::endl;
}

void Print6()
{

    {
        ionic::Table table;
        AddVar4Rows(table);
        table.print();
        std::cout << std::endl;
    }
    {
        ionic::TableOptions options;
        options.outerBorder = false;
        options.innerHDivider = true;
        PrintDiffs(options);

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        std::cout << std::endl;
    }
    {
        ionic::TableOptions options;
        options.outerBorder = true;
        options.innerHDivider = false;
        PrintDiffs(options);

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        std::cout << std::endl;
    }
    {
        ionic::TableOptions options;
        options.outerBorder = false;
        options.innerHDivider = false;
        PrintDiffs(options);

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        std::cout << std::endl;
    }
    {
        ionic::TableOptions options;
        options.outerBorder = true;
        options.innerHDivider = false;
        options.innerVDivider = false;
        PrintDiffs(options);

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        std::cout << std::endl;
    }
    {
        ionic::TableOptions options;
        options.outerBorder = false;
        options.innerHDivider = false;
        options.innerVDivider = false;
        PrintDiffs(options);

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        std::cout << std::endl;
    }
}

#define TEST(x)                                                 \
	if (!(x)) {	                                                \
		printf("ERROR: line %d in %s\n", __LINE__, __FILE__);   \
        assert(false);                                          \
		return false;										    \
	}

namespace ionic {
class IonicTest {
public:
    static bool test();
};

bool IonicTest::test()
{
    using namespace ionic;
    {
        std::string t = "This\r\nis multi-line\n\rstring\n\r  \n";
        Table::normalizeNL(t);
        TEST(t.find('\r') == std::string::npos);
        TEST(!t.empty());

        Table::trimRight(t);
        TEST(t == "This\nis multi-line\nstring");

        int max = 0;
        int nSub = Table::nLines(t, max);
        TEST(nSub == 3);
        TEST(max == 13);
    }
    {
        std::string t = "Hello";
        int max = 0;
        int nSub = Table::nLines(t, max);
        TEST(nSub == 1);
        TEST(max == 5);
    }
    {
        //                   0    5    10   15   20   25   30   35   40
        std::string line = "This is a test.";

        Table::Break r = Table::lineBreak(line, 0, line.size(), 15);
        TEST(r.start == 0);
        TEST(r.end == 15);
        TEST(r.next == 15);

        r = Table::lineBreak(line, 0, line.size(), 100);
        TEST(r.start == 0);
        TEST(r.end == 15);
        TEST(r.next == 15);

        for (int s = 1; s < 6; s++) {
            r = Table::lineBreak(line, 0, line.size(), s);
            TEST(r.start == 0 && r.end == 4 && r.next == 5);
        }
        r = Table::lineBreak(line, 0, line.size(), 5);
        TEST(r.start == 0 && r.end == 4 && r.next == 5);
        for (int s = 7; s < 9; s++) {
            r = Table::lineBreak(line, 0, line.size(), s);
            TEST(r.start == 0 && r.end == 7 && r.next == 8);
        }
        for (int s = 9; s < 15; s++) {
            r = Table::lineBreak(line, 0, line.size(), s);
            TEST(r.start == 0 && r.end == 9 && r.next == 10);
        }

        r = Table::lineBreak(line, 0, line.size(), 15);
        TEST(r.start == 0 && r.end == 15 && r.next == 15);
    }
    {
        //                   0    5
        std::string line = "Test  ";
        Table::Break r = Table::lineBreak(line, 0, line.size(), 100);
        TEST(r.start == 0);
        TEST(r.end == 4);
        TEST(r.next == 6);
    }
    {
        //                   0    5    10   15   20   25   30   35   40
        std::string line = "Prev. Test  ";
        Table::Break r = Table::lineBreak(line, 6, line.size(), 100);
        TEST(r.start == 6);
        TEST(r.end == 10);
        TEST(r.next == line.size());
    }
    {
        //                   0    5    10   15   20   25   30   35   40   45   50   55   60   65   70   75   80   85   90   95   100
        std::string line = "It was a bright cold day in April, and the clocks were striking thirteen.";
        //                  "It was a bright"	0, 15, 16
        //                  "cold day in"       16, 27, 28
        //                  "April, and the"    28, 42, 43
        //                  "clocks were"       43, 54, 55
        //                  "striking"          55, 63, 64
        //                  "thirteen."	        64, 73, 73
        std::vector<Table::Break> breaks = Table::wordWrap(line, 15);
        TEST(breaks.size() == 6);
        TEST(breaks[0].start == 0 && breaks[0].end == 15 && breaks[0].next == 16);
        TEST(breaks[1].start == 16 && breaks[1].end == 27 && breaks[1].next == 28);
        TEST(breaks[2].start == 28 && breaks[2].end == 42 && breaks[2].next == 43);
        TEST(breaks[3].start == 43 && breaks[3].end == 54 && breaks[3].next == 55);
        TEST(breaks[4].start == 55 && breaks[4].end == 63 && breaks[4].next == 64);
        TEST(breaks[5].start == 64 && breaks[5].end == 73 && breaks[5].next == 73);
    }
    {
        //                  0    5 _ _  10   15   20_   25   30   35 _  40   45   50   55   60   65   70   75   80   85   90   95   100
        std::string line = "A Poem.\n\nTo challenge\nthe line breaker\n";
        std::vector<Table::Break> breaks = Table::wordWrap(line, 15);
        TEST(breaks.size() == 5);
        TEST(breaks[0].start == 0 && breaks[0].end == 7 && breaks[0].next == 8);
        TEST(breaks[1].start == 8 && breaks[1].end == 8 && breaks[1].next == 9);
        TEST(breaks[2].start == 9 && breaks[2].end == 21 && breaks[2].next == 22);
        TEST(breaks[3].start == 22 && breaks[3].end == 30 && breaks[3].next == 31);
        TEST(breaks[4].start == 31 && breaks[4].end == 38 && breaks[4].next == 39);
    }
    {
        // Line breaking used by markdown and similar.
        // Notes:
        // - a new line is a space
        // - but it's very common to have a space at the end of a line
        // - a double new line is a paragraph break
        std::string line =
            "A group searches \n"
            "in the Antarctic. It is a long and perilous\n"
            "journey.\n"
            " \n"
            "They are unprepared for the\n"
            "strange secrets that they uncover.";

        std::string out = ionic::Table::normalizeMD(line, 1);
        std::string expected =
            "A group searches in the Antarctic. It is a long and perilous journey.\n"
            "They are unprepared for the strange secrets that they uncover.";
        TEST(out == expected);

        // BUT! Sometime we expect the double new line to be preserved.
        // Markdown is tricky stuff.
        std::string out2 = ionic::Table::normalizeMD(line, 2);
        std::string expected2 =
            "A group searches in the Antarctic. It is a long and perilous journey.\n\n"
            "They are unprepared for the strange secrets that they uncover.";
        TEST(out2 == expected2);
    }
    {
        // Bug fix
        std::string s =
            "AAA\n"
            "\n"
            "BBB\n"
            "\n"
            "CCC\n\n";

        std::string out1 = ionic::Table::normalizeMD(s, 1);
        TEST(out1 == "AAA\nBBB\nCCC");

        std::string out2 = ionic::Table::normalizeMD(s, 2);
        TEST(out2 == "AAA\n\nBBB\n\nCCC");

        std::vector<Table::Break> breaks1 = Table::wordWrap(out1, 100);
        TEST(breaks1.size() == 3);
        TEST(breaks1[0].start == 0 && breaks1[0].end == 3 && breaks1[0].next == 4);
        TEST(breaks1[1].start == 4 && breaks1[1].end == 7 && breaks1[1].next == 8);
        TEST(breaks1[2].start == 8 && breaks1[2].end == 11 && breaks1[2].next == 11);

        std::vector<Table::Break> breaks2 = Table::wordWrap(out2, 100);
        TEST(breaks2.size() == 5);
        TEST(breaks2[0].start == 0 && breaks2[0].end == 3 && breaks2[0].next == 4);
        TEST(breaks2[1].start == 4 && breaks2[1].end == 4 && breaks2[1].next == 5);
        TEST(breaks2[2].start == 5 && breaks2[2].end == 8 && breaks2[2].next == 9);
        TEST(breaks2[3].start == 9 && breaks2[3].end == 9 && breaks2[3].next == 10);
        TEST(breaks2[4].start == 10 && breaks2[4].end == 13 && breaks2[4].next == 13);

        ionic::Table t1;
        t1.addRow({ out1 });
        std::string f1 = t1.format();
        TEST(f1 == 
            "+-----+\n"
            "| AAA |\n"
			"| BBB |\n"
            "| CCC |\n"
			"+-----+\n");

        ionic::Table t2;
		t2.addRow({ out2 });
		std::string f2 = t2.format();
        TEST(f2 ==
            "+-----+\n"
            "| AAA |\n"
            "|     |\n"
            "| BBB |\n"
            "|     |\n"
            "| CCC |\n"
            "+-----+\n");
    }
    return true;
}
}

int main(int argc, const char* argv[]) 
{
    bool printAllTests = false;
    for (int i = 0; i < argc; ++i) {
        if (std::string("-v") == argv[i])
			printAllTests = true;
    }

    ionic::Table::initConsole();
    bool okay = ionic::IonicTest::test();
    if (okay)
        std::cout << "All tests passed.\n";
    else
        std::cout << "ERROR tests failed.\n";

    std::cout << "Welcome to ionic. (https://github.com/leethomason/ionic)\nA simple table formatter for console output in c++.\n\n";

    Print6();
    
    if (printAllTests) {
        ionic::TableOptions options;
        options.maxWidth = 50;
        ionic::Table t5(options);
        t5.setColumnFormat({ {ionic::ColType::fixed, 1},
                             {ionic::ColType::fixed, 4},
                             {ionic::ColType::flex},
                             {ionic::ColType::flex},
                             {ionic::ColType::flex} });
        t5.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
        t5.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
        t5.print();
        PrintRuler(options.maxWidth);
    }
    {
        ionic::TableOptions options;
        options.tableColor = ionic::Color::blue;
        ionic::Table t(options);

        t.addRow({ "", "R", "G", "B", "Y", "M", "C" });
        t.addRow({ "Normal", "Red", "Green", "Blue", "Yellow", "Magenta", "Cyan" });
        t.addRow({ "Bright", "Red", "Green", "Blue", "Yellow", "Magenta", "Cyan" });

        t.setRow(0, { ionic::Color::white }, {});
        t.setCell(2, 0, { ionic::Color::white }, {});

        t.setCell(1, 1, { ionic::Color::red }, {});
        t.setCell(2, 1, { ionic::Color::brightRed }, {});
        t.setCell(1, 2, { ionic::Color::green }, {});
        t.setCell(2, 2, { ionic::Color::brightGreen }, {});
        t.setCell(1, 3, { ionic::Color::blue }, {});
        t.setCell(2, 3, { ionic::Color::brightBlue }, {});
        t.setCell(1, 4, { ionic::Color::yellow }, {});
        t.setCell(2, 4, { ionic::Color::brightYellow }, {});
        t.setCell(1, 5, { ionic::Color::magenta }, {});
        t.setCell(2, 5, { ionic::Color::brightMagenta }, {});
        t.setCell(1, 6, { ionic::Color::cyan }, {});
        t.setCell(2, 6, { ionic::Color::brightCyan }, {});

        t.print();
    }

    {
        ionic::Table t;
        t.setColumnFormat({ {ionic::ColType::fixed, 10}, {ionic::ColType::fixed, 10}, {ionic::ColType::fixed, 10} });
        t.addRow({ { "This is left aligned text" }, { "This text is center aligned" }, { "And finally this is right aligned" } });
        t.setColumn(0, {ionic::Color::brightGreen}, { ionic::Alignment::left });
        t.setColumn(1, {ionic::Color::yellow}, { ionic::Alignment::center });
        t.setColumn(2, {ionic::Color::brightCyan}, { ionic::Alignment::right });
        t.print();
    }

    if (printAllTests) {
        // These are useful if really changing code, but noisy to look at.
        {
            ionic::TableOptions options;
            options.maxWidth = 80;
            ionic::Table t(options);
            t.setColumnFormat({ {ionic::ColType::fixed, 1},
                                 {ionic::ColType::fixed, 4},
                                 {ionic::ColType::flex},
                                 {ionic::ColType::flex},
                                 {ionic::ColType::flex} });
            t.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
            t.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
            t.print();
            PrintRuler(options.maxWidth);
        }
        {
            ionic::Table t6;
            t6.setColumnFormat({ {ionic::ColType::fixed, 1},
                                 {ionic::ColType::fixed, 4},
                                 {ionic::ColType::flex},
                                 {ionic::ColType::flex},
                                 {ionic::ColType::flex} });
            t6.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
            t6.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
            t6.print();
        }
        {
            ionic::TableOptions options;
            options.borderHChar = '=';
            options.borderVChar = 'I';
            options.borderCornerChar = 'O';
            ionic::Table t(options);;
            t.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
            t.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
            t.print();
        }
        {
            ionic::Table t;
            t.setColumnFormat({ {ionic::ColType::fixed, 15} });
            t.addRow({ "123456789012345" });
            t.addRow({ "It was a bright cold day in April, and the clocks were striking thirteen." });
            std::cout << t;
        }
    }
    return okay ? 0 : 1;
}