
#include "ionic/ionic.h"
#include <fmt/core.h>


void PrintRuler(int w)
{
    for (int i = 0; i < w; i++) {
		fmt::print("{}", i%10);
	}
    fmt::print("\n");
}

void AddVar4Rows(ionic::Table& table)
{
    table.setColumnFormat({ {ionic::ColType::kFixed, 2}, {ionic::ColType::kDynamic}, {ionic::ColType::kDynamic} });
    table.addRow({ "0", "A", "The Outer World" });
    table.addRow({ "1", "Hello", "And Another" });
}

void Print4()
{
    ionic::TableOptions options;

    {
        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        fmt::print("\n\n");
    }
    {
        options.outerBorder = false;
        options.innerHorizontalDivider = true;

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        fmt::print("\n\n");
    }
    {
        options.outerBorder = true;
        options.innerHorizontalDivider = false;

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        fmt::print("\n\n");
    }
    {
        options.outerBorder = false;
        options.innerHorizontalDivider = false;

        ionic::Table table(options);
        AddVar4Rows(table);
        table.print();
        fmt::print("\n\n");
    }
}

int main(const char* argv[], int argc) {
    (void)argv;
    (void)argc;
    //fmt::print("Hello, World!");

    ionic::Table::initConsole();
    ionic::Table::test();

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
        ionic::Table t6;
        t6.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
        t6.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
        t6.print();
    }
    {
        ionic::Table t6;
        t6.setColumnFormat({ {ionic::ColType::kFixed, 15} });
        t6.addRow({ "123456789012345" });
        t6.addRow({ "It was a bright cold day in April, and the clocks were striking thirteen." });
        t6.print();
    }
    return 0;
}