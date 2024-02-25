
#include "ionic/ionic.h"
#include <fmt/core.h>


static constexpr char* test0 =
    "Call me Ishmael. Some years ago — never mind how long precisely — having little or no "
    "money in my purse, and nothing particular to interest me on shore, I thought I would"
    " sail about a little and see the watery part of the world.";

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

    t2.outerBorder = false;
    t2.innerHorizontalDivider = true;
    t2.print();
    fmt::print("\n\n");

    t3.outerBorder = true;
    t3.innerHorizontalDivider = false;
    t3.print();
    fmt::print("\n\n");

    t4.outerBorder = false;
    t4.innerHorizontalDivider = false;
    t4.print();
    fmt::print("\n\n");

    ionic::Ionic t5;
    static const int kWidth = 50;
    t5.maxWidth = kWidth;
    t5.setColumnFormat({ {ionic::ColType::kFixed, 1}, 
                         {ionic::ColType::kFixed, 4},
                         {ionic::ColType::kDynamic},
                         {ionic::ColType::kDynamic}, 
                         {ionic::ColType::kDynamic} });
    t5.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
    t5.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen."});
    t5.print();
    fmt::print("{:>{}}", ".", kWidth);

    return 0;
}