
#include "ionic/ionic.h"
#include <fmt/core.h>


void PrintRuler(int w)
{
    for (int i = 0; i < w; i++) {
		fmt::print("{}", i%10);
	}
    fmt::print("\n");
}


int main(const char* argv[], int argc) {
    (void)argv;
    (void)argc;
    //fmt::print("Hello, World!");

    ionic::Ionic::initConsole();
    ionic::Ionic::test();

    ionic::Ionic table;

    table.setColumnFormat({ {ionic::ColType::kFixed, 2}, {ionic::ColType::kDynamic}, {ionic::ColType::kDynamic} });
    table.addRow({ "0", "A", "The Outer World" });
    table.addRow({ "1", "Hello", "And Another" });

    table.print();
    fmt::print("\n\n");

    ionic::Ionic t2 = table;
    ionic::Ionic t3 = table;
    ionic::Ionic t4 = table;

    t2.options.outerBorder = false;
    t2.options.innerHorizontalDivider = true;
    t2.print();
    fmt::print("\n\n");

    t3.options.outerBorder = true;
    t3.options.innerHorizontalDivider = false;
    t3.print();
    fmt::print("\n\n");

    t4.options.outerBorder = false;
    t4.options.innerHorizontalDivider = false;
    t4.print();
    fmt::print("\n\n");
    
    {
        ionic::Ionic t5;
        static const int kWidth = 50;
        t5.options.maxWidth = kWidth;
        t5.setColumnFormat({ {ionic::ColType::kFixed, 1},
                             {ionic::ColType::kFixed, 4},
                             {ionic::ColType::kDynamic},
                             {ionic::ColType::kDynamic},
                             {ionic::ColType::kDynamic} });
        t5.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
        t5.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
        t5.print();
        PrintRuler(kWidth);
    }
    {
        ionic::Ionic t6;
        static const int kWidth = 80;
        t6.options.maxWidth = kWidth;
        t6.setColumnFormat({ {ionic::ColType::kFixed, 1},
                             {ionic::ColType::kFixed, 4},
                             {ionic::ColType::kDynamic},
                             {ionic::ColType::kDynamic},
                             {ionic::ColType::kDynamic} });
        t6.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
        t6.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
        t6.print();
        PrintRuler(kWidth);
    }
    {
        ionic::Ionic t6;
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
        ionic::Ionic t6;
        t6.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
        t6.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
        t6.print();
    }
    {
        ionic::Ionic t6;
        t6.setColumnFormat({ {ionic::ColType::kFixed, 15} });
        t6.addRow({ "123456789012345" });
        t6.addRow({ "It was a bright cold day in April, and the clocks were striking thirteen." });
        t6.print();
    }
    return 0;
}