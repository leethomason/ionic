#include <gtest/gtest.h>
#include "ionic/ionic.h"

#include <numeric>

namespace ionic {

// IonicTest is declared a friend of Table in ionic.h, giving access to
// private static methods. TEST_F tests that need private access inherit
// from this fixture and call helpers defined here.
class IonicTest : public ::testing::Test {
protected:
    static void trimRight(std::string& s)       { Table::trimRight(s); }
    static void normalizeNL(std::string& s)     { Table::normalizeNL(s); }
    static int  nLines(const std::string& s, int& maxWidth) { return Table::nLines(s, maxWidth); }
    static Table::Break lineBreak(const std::string& text, size_t start, size_t end, int width) {
        return Table::lineBreak(text, start, end, width);
    }
};

// ---------------------------------------------------------------------------
// String utilities
// ---------------------------------------------------------------------------

TEST_F(IonicTest, TrimRight_NoChange) {
    std::string t = "Test";
    trimRight(t);
    EXPECT_EQ(t, "Test");
}

TEST_F(IonicTest, TrimRight_AllWhitespace) {
    std::string t = "  ";
    trimRight(t);
    EXPECT_TRUE(t.empty());
}

TEST_F(IonicTest, TrimRight_TrailingSpaces) {
    std::string t = " aa  ";
    trimRight(t);
    EXPECT_EQ(t, " aa");
}

TEST_F(IonicTest, NormalizeNL_RemovesCR) {
    std::string t = "This\r\nis multi-line\n\rstring\n\r  \n";
    normalizeNL(t);
    EXPECT_EQ(t.find('\r'), std::string::npos);
    EXPECT_FALSE(t.empty());

    trimRight(t);
    EXPECT_EQ(t, "This\nis multi-line\nstring");
}

TEST_F(IonicTest, NLines_Multiline) {
    std::string t = "This\nis multi-line\nstring";
    int maxWidth = 0;
    EXPECT_EQ(nLines(t, maxWidth), 3);
    EXPECT_EQ(maxWidth, 13);
}

TEST_F(IonicTest, NLines_SingleLine) {
    std::string t = "Hello";
    int maxWidth = 0;
    EXPECT_EQ(nLines(t, maxWidth), 1);
    EXPECT_EQ(maxWidth, 5);
}

// ---------------------------------------------------------------------------
// LineBreak
// ---------------------------------------------------------------------------

TEST_F(IonicTest, LineBreak_ExactFit) {
    std::string line = "This is a test.";
    auto r = lineBreak(line, 0, line.size(), 15);
    EXPECT_EQ(r.start, 0u);
    EXPECT_EQ(r.end,   15u);
    EXPECT_EQ(r.next,  15u);
}

TEST_F(IonicTest, LineBreak_WiderThanText) {
    std::string line = "This is a test.";
    auto r = lineBreak(line, 0, line.size(), 100);
    EXPECT_EQ(r.start, 0u);
    EXPECT_EQ(r.end,   15u);
    EXPECT_EQ(r.next,  15u);
}

TEST_F(IonicTest, LineBreak_NarrowBreaksAtFirstWord) {
    std::string line = "This is a test.";
    for (int w = 1; w <= 5; ++w) {
        auto r = lineBreak(line, 0, line.size(), w);
        EXPECT_EQ(r.start, 0u)  << "width=" << w;
        EXPECT_EQ(r.end,   4u)  << "width=" << w;
        EXPECT_EQ(r.next,  5u)  << "width=" << w;
    }
}

TEST_F(IonicTest, LineBreak_BreaksAtSecondWord) {
    std::string line = "This is a test.";
    for (int w = 7; w <= 8; ++w) {
        auto r = lineBreak(line, 0, line.size(), w);
        EXPECT_EQ(r.start, 0u)  << "width=" << w;
        EXPECT_EQ(r.end,   7u)  << "width=" << w;
        EXPECT_EQ(r.next,  8u)  << "width=" << w;
    }
}

TEST_F(IonicTest, LineBreak_BreaksAtThirdWord) {
    std::string line = "This is a test.";
    for (int w = 9; w <= 14; ++w) {
        auto r = lineBreak(line, 0, line.size(), w);
        EXPECT_EQ(r.start, 0u)  << "width=" << w;
        EXPECT_EQ(r.end,   9u)  << "width=" << w;
        EXPECT_EQ(r.next,  10u) << "width=" << w;
    }
}

TEST_F(IonicTest, LineBreak_TrailingSpaces) {
    std::string line = "Test  ";
    auto r = lineBreak(line, 0, line.size(), 100);
    EXPECT_EQ(r.start, 0u);
    EXPECT_EQ(r.end,   4u);
    EXPECT_EQ(r.next,  6u);
}

TEST_F(IonicTest, LineBreak_Offset) {
    std::string line = "Prev. Test  ";
    auto r = lineBreak(line, 6, line.size(), 100);
    EXPECT_EQ(r.start, 6u);
    EXPECT_EQ(r.end,   10u);
    EXPECT_EQ(r.next,  line.size());
}

// ---------------------------------------------------------------------------
// WordWrap
// ---------------------------------------------------------------------------

TEST(WordWrap, Sentence) {
    std::string line = "It was a bright cold day in April, and the clocks were striking thirteen.";
    auto breaks = Table::wordWrap(line, 15);
    ASSERT_EQ(breaks.size(), 6u);
    EXPECT_EQ(breaks[0].start,  0u);  EXPECT_EQ(breaks[0].end, 15u); EXPECT_EQ(breaks[0].next, 16u);
    EXPECT_EQ(breaks[1].start, 16u);  EXPECT_EQ(breaks[1].end, 27u); EXPECT_EQ(breaks[1].next, 28u);
    EXPECT_EQ(breaks[2].start, 28u);  EXPECT_EQ(breaks[2].end, 42u); EXPECT_EQ(breaks[2].next, 43u);
    EXPECT_EQ(breaks[3].start, 43u);  EXPECT_EQ(breaks[3].end, 54u); EXPECT_EQ(breaks[3].next, 55u);
    EXPECT_EQ(breaks[4].start, 55u);  EXPECT_EQ(breaks[4].end, 63u); EXPECT_EQ(breaks[4].next, 64u);
    EXPECT_EQ(breaks[5].start, 64u);  EXPECT_EQ(breaks[5].end, 73u); EXPECT_EQ(breaks[5].next, 73u);
}

TEST(WordWrap, EmbeddedNewlines) {
    std::string line = "A Poem.\n\nTo challenge\nthe line breaker\n";
    auto breaks = Table::wordWrap(line, 15);
    ASSERT_EQ(breaks.size(), 5u);
    EXPECT_EQ(breaks[0].start,  0u); EXPECT_EQ(breaks[0].end,  7u); EXPECT_EQ(breaks[0].next,  8u);
    EXPECT_EQ(breaks[1].start,  8u); EXPECT_EQ(breaks[1].end,  8u); EXPECT_EQ(breaks[1].next,  9u);
    EXPECT_EQ(breaks[2].start,  9u); EXPECT_EQ(breaks[2].end, 21u); EXPECT_EQ(breaks[2].next, 22u);
    EXPECT_EQ(breaks[3].start, 22u); EXPECT_EQ(breaks[3].end, 30u); EXPECT_EQ(breaks[3].next, 31u);
    EXPECT_EQ(breaks[4].start, 31u); EXPECT_EQ(breaks[4].end, 38u); EXPECT_EQ(breaks[4].next, 39u);
}

// ---------------------------------------------------------------------------
// Color
// ---------------------------------------------------------------------------

TEST(Color, Colorize) {
    Table::colorEnabled = true;
    std::string t = Table::colorize(Color::red, "Hello");
    EXPECT_EQ(t, "\033[31mHello\033[0m");
}

TEST(Color, StrToColor_DarkVariants) {
    EXPECT_EQ(strToColor("red"),          Color::red);
    EXPECT_EQ(strToColor("DARK_GREEN"),   Color::green);
    EXPECT_EQ(strToColor("dark yellow"),  Color::yellow);
    EXPECT_EQ(strToColor("darkBlue"),     Color::blue);
    EXPECT_EQ(strToColor("Magenta"),      Color::magenta);
    EXPECT_EQ(strToColor("cyan"),         Color::cyan);
}

TEST(Color, StrToColor_BrightVariants) {
    EXPECT_EQ(strToColor("brightRed"),      Color::brightRed);
    EXPECT_EQ(strToColor("bright green"),   Color::brightGreen);
    EXPECT_EQ(strToColor("PALE_YELLOW"),    Color::brightYellow);
    EXPECT_EQ(strToColor("LIGHT-BLUE"),     Color::brightBlue);
    EXPECT_EQ(strToColor("pale magenta"),   Color::brightMagenta);
    EXPECT_EQ(strToColor("BrightCyan"),     Color::brightCyan);
}

TEST(Color, StrToColor_NeutralAndFallback) {
    EXPECT_EQ(strToColor("Black"),       Color::black);
    EXPECT_EQ(strToColor("grey"),        Color::gray);
    EXPECT_EQ(strToColor("light gray"),  Color::brightGray);
    EXPECT_EQ(strToColor("WHITE"),       Color::white);
    EXPECT_EQ(strToColor("reset"),       Color::reset);
    EXPECT_EQ(strToColor("default"),     Color::reset);
    EXPECT_EQ(strToColor("foobar"),      Color::reset);
}

// ---------------------------------------------------------------------------
// Table formatting
// ---------------------------------------------------------------------------

TEST(Table, TwoColumnNoBorders) {
    TableOptions options;
    options.border   = false;
    options.hDivider = false;

    ionic::Table t(options);
    t.addRow({ "AA", "Hello" });
    t.addRow({ "BB", "World" });
    EXPECT_EQ(t.format(), "AA | Hello\nBB | World\n");
}

TEST(Table, BasicFormat) {
    ionic::Table table;
    table.setColumns({ {2}, {0}, {0} });
    table.addRow({ "0", "A",     "The Outer World" });
    table.addRow({ "1", "Hello", "And Another"     });
    table.addRow({ "2", "World", "Farther Out"     });

    const std::string expected =
        "+----+-------+-----------------+\n"
        "| 0  | A     | The Outer World |\n"
        "+----+-------+-----------------+\n"
        "| 1  | Hello | And Another     |\n"
        "+----+-------+-----------------+\n"
        "| 2  | World | Farther Out     |\n"
        "+----+-------+-----------------+\n";

    EXPECT_EQ(table.format(), expected);
}

TEST(Table, FormatRowsMatchesFormat) {
    ionic::Table table;
    table.setColumns({ {2}, {0}, {0} });
    table.addRow({ "0", "A",     "The Outer World" });
    table.addRow({ "1", "Hello", "And Another"     });
    table.addRow({ "2", "World", "Farther Out"     });

    std::string full = table.format();
    auto rows = table.formatRows();
    std::string joined = std::accumulate(rows.begin(), rows.end(), std::string());
    EXPECT_EQ(joined, full);
}

TEST(Table, NoHDivider) {
    TableOptions options;
    options.hDivider = false;
    ionic::Table table(options);
    table.setColumns({ {2}, {0}, {0} });
    table.addRow({ "0", "A",     "The Outer World" });
    table.addRow({ "1", "Hello", "And Another"     });
    table.addRow({ "2", "World", "Farther Out"     });

    const std::string expected =
        "+----+-------+-----------------+\n"
        "| 0  | A     | The Outer World |\n"
        "| 1  | Hello | And Another     |\n"
        "| 2  | World | Farther Out     |\n"
        "+----+-------+-----------------+\n";

    EXPECT_EQ(table.format(), expected);

    auto rows = table.formatRows();
    std::string joined = std::accumulate(rows.begin(), rows.end(), std::string());
    EXPECT_EQ(joined, expected);
}

}  // namespace ionic
