
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
    table.setColumnFormat({ {ionic::ColType::kFixed, 2}, {ionic::ColType::kDynamic}, {ionic::ColType::kDynamic} });
    table.addRow({ "0", "A", "The Outer World" });
    table.addRow({ "1", "Hello", "And Another" });
    table.addRow({ "2", "World", "Further Out" });
}

void Print4()
{
    ionic::TableOptions options;

    {
        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        std::cout << std::endl << std::endl;
    }
    {
        options.outerBorder = false;
        options.innerHorizontalDivider = true;

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        std::cout << std::endl << std::endl;
    }
    {
        options.outerBorder = true;
        options.innerHorizontalDivider = false;

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        std::cout << std::endl << std::endl;
    }
    {
        options.outerBorder = false;
        options.innerHorizontalDivider = false;

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        std::cout << std::endl << std::endl;
    }
}

#define TEST(x)                                                 \
	if (!(x)) {	                                                \
		printf("ERROR: line %d in %s\n", __LINE__, __FILE__);   \
        assert(false);                                          \
		return false;										    \
	}

bool test()
{
    using namespace ionic;
    {
        std::string t0 = "This\r\nis multi-line\n\rstring\n\r  \n";
        Table::normalizeNL(t0);
        TEST(t0.find('\r') == std::string::npos);
        TEST(!t0.empty());

        Table::trimRight(t0);
        TEST(t0 == "This\nis multi-line\nstring");

        int max = 0;
        int nSub = Table::nLines(t0, max);
        TEST(nSub == 3);
        TEST(max == 13);
    }
    {
        std::string t1 = "Hello";
        int max = 0;
        int nSub = Table::nLines(t1, max);
        TEST(nSub == 1);
        TEST(max == 5);
    }
    {
        //                   0    5    10   15   20   25   30   35   40
        std::string line0 = "This is a test.";

        Table::Break r = Table::lineBreak(line0, 0, line0.size(), 15);
        TEST(r.start == 0);
        TEST(r.end == 15);
        TEST(r.next == 15);

        r = Table::lineBreak(line0, 0, line0.size(), 100);
        TEST(r.start == 0);
        TEST(r.end == 15);
        TEST(r.next == 15);

        for (int s = 1; s < 6; s++) {
            r = Table::lineBreak(line0, 0, line0.size(), s);
            TEST(r.start == 0 && r.end == 4 && r.next == 5);
        }
        r = Table::lineBreak(line0, 0, line0.size(), 5);
        TEST(r.start == 0 && r.end == 4 && r.next == 5);
        for (int s = 7; s < 9; s++) {
            r = Table::lineBreak(line0, 0, line0.size(), s);
            TEST(r.start == 0 && r.end == 7 && r.next == 8);
        }
        for (int s = 9; s < 15; s++) {
            r = Table::lineBreak(line0, 0, line0.size(), s);
            TEST(r.start == 0 && r.end == 9 && r.next == 10);
        }

        r = Table::lineBreak(line0, 0, line0.size(), 15);
        TEST(r.start == 0 && r.end == 15 && r.next == 15);
    }
    {
        //                   0    5
        std::string line1 = "Test  ";
        Table::Break r = Table::lineBreak(line1, 0, line1.size(), 100);
        TEST(r.start == 0);
        TEST(r.end == 4);
        TEST(r.next == 6);
    }
    {
        //                   0    5    10   15   20   25   30   35   40
        std::string line2 = "Prev. Test  ";
        Table::Break r = Table::lineBreak(line2, 6, line2.size(), 100);
        TEST(r.start == 6);
        TEST(r.end == 10);
        TEST(r.next == line2.size());
    }
    {
        //                   0    5    10   15   20   25   30   35   40   45   50   55   60   65   70   75   80   85   90   95   100
        std::string line3 = "It was a bright cold day in April, and the clocks were striking thirteen.";
        //                  "It was a bright"	0, 15, 16
        //                  "cold day in"       16, 27, 28
        //                  "April, and the"    28, 42, 43
        //                  "clocks were"       43, 54, 55
        //                  "striking"          55, 63, 64
        //                  "thirteen."	        64, 73, 73
        std::vector<Table::Break> breaks = Table::wordWrap(line3, 15);
        TEST(breaks.size() == 6);
        TEST(breaks[0].start == 0 && breaks[0].end == 15 && breaks[0].next == 16);
        TEST(breaks[1].start == 16 && breaks[1].end == 27 && breaks[1].next == 28);
        TEST(breaks[2].start == 28 && breaks[2].end == 42 && breaks[2].next == 43);
        TEST(breaks[3].start == 43 && breaks[3].end == 54 && breaks[3].next == 55);
        TEST(breaks[4].start == 55 && breaks[4].end == 63 && breaks[4].next == 64);
        TEST(breaks[5].start == 64 && breaks[5].end == 73 && breaks[5].next == 73);
    }
    {
        //                   0    5 _ _  10   15   20_   25   30   35 _  40   45   50   55   60   65   70   75   80   85   90   95   100
        std::string line4 = "A Poem.\n\nTo challenge\nthe line breaker\n";
        std::vector<Table::Break> breaks = Table::wordWrap(line4, 15);
        TEST(breaks.size() == 5);
        TEST(breaks[0].start == 0 && breaks[0].end == 7 && breaks[0].next == 8);
        TEST(breaks[1].start == 8 && breaks[1].end == 9 && breaks[1].next == 9);
        TEST(breaks[2].start == 9 && breaks[2].end == 21 && breaks[2].next == 22);
        TEST(breaks[3].start == 22 && breaks[3].end == 30 && breaks[3].next == 31);
        TEST(breaks[4].start == 31 && breaks[4].end == 38 && breaks[4].next == 39);
    }
    return true;
}

int main(const char* argv[], int argc) {
    (void)argv;
    (void)argc;
    //fmt::print("Hello, World!");

    ionic::Table::initConsole();
    bool okay = test();
    if (okay)
        std::cout << "All tests passed.\n";
    else
        std::cout << "ERROR tests failed.\n";

    Print4();
    
    {
        ionic::TableOptions options;
        options.maxWidth = 50;
        ionic::Table t5(options);
        t5.setColumnFormat({ {ionic::ColType::kFixed, 1},
                             {ionic::ColType::kFixed, 4},
                             {ionic::ColType::kDynamic},
                             {ionic::ColType::kDynamic},
                             {ionic::ColType::kDynamic} });
        t5.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
        t5.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
        t5.print();
        PrintRuler(options.maxWidth);
    }
#if 0
    {
        ionic::TableOptions options;
        options.maxWidth = 80;
        ionic::Table t6(options);
        t6.setColumnFormat({ {ionic::ColType::kFixed, 1},
                             {ionic::ColType::kFixed, 4},
                             {ionic::ColType::kDynamic},
                             {ionic::ColType::kDynamic},
                             {ionic::ColType::kDynamic} });
        t6.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
        t6.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
        t6.print();
        PrintRuler(options.maxWidth);
    }
    {
        ionic::Table t6;
        t6.setColumnFormat({ {ionic::ColType::kFixed, 1},
                             {ionic::ColType::kFixed, 4},
                             {ionic::ColType::kDynamic},
                             {ionic::ColType::kDynamic},
                             {ionic::ColType::kDynamic} });
        t6.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
        t6.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
        t6.print();
    }
    {
        ionic::TableOptions options;
        options.borderHChar = '=';
        options.borderVChar = 'I';
        options.borderCornerChar = 'O';
        ionic::Table t6(options);;
        t6.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
        t6.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
        t6.print();
    }
    {
        ionic::Table t6;
        t6.setColumnFormat({ {ionic::ColType::kFixed, 15} });
        t6.addRow({ "123456789012345" });
        t6.addRow({ "It was a bright cold day in April, and the clocks were striking thirteen." });
        std::cout << t6;
    }
#endif
    return 0;
}