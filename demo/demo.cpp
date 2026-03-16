
#include "ionic/ionic.h"

#include <iostream>

void PrintRuler(int w)
{
	for (int i = 0; i < w; i++) {
		std::cout << i % 10;
	}
	std::cout << std::endl;
}

void AddVar4Rows(ionic::Table& table)
{
	table.setColumns({ {2}, {0}, {0} });
	table.addRow({ "0", "A", "The Outer World" });
	table.addRow({ "1", "Hello", "And Another" });
	table.addRow({ "2", "World", "Farther Out" });
}

void PrintDiffs(const ionic::TableOptions& options)
{
	ionic::TableOptions def;
	if (options.border != def.border)
		std::cout << "outerBorder: " << options.border << std::endl;
	if (options.hDivider != def.hDivider)
		std::cout << "innerHDivider: " << options.hDivider << std::endl;
	if (options.vDivider != def.vDivider)
		std::cout << "innerVDivider: " << options.vDivider << std::endl;
	if (options.hChar != def.hChar)
		std::cout << "borderHChar: " << options.hChar << std::endl;
	if (options.vChar != def.vChar)
		std::cout << "borderVChar: " << options.vChar << std::endl;
	if (options.cornerChar != def.cornerChar)
		std::cout << "borderCornerChar: " << options.cornerChar << std::endl;
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
		options.border = false;
		options.hDivider = true;
		PrintDiffs(options);

		ionic::Table table(options);
		AddVar4Rows(table);
		table.print();
		std::cout << std::endl;
	}
	{
		ionic::TableOptions options;
		options.border = true;
		options.hDivider = false;
		PrintDiffs(options);

		ionic::Table table(options);
		AddVar4Rows(table);
		table.print();
		std::cout << std::endl;
	}
	{
		ionic::TableOptions options;
		options.border = false;
		options.hDivider = false;
		PrintDiffs(options);

		ionic::Table table(options);
		AddVar4Rows(table);
		table.print();
		std::cout << std::endl;
	}
	{
		ionic::TableOptions options;
		options.border = true;
		options.hDivider = false;
		options.vDivider = false;
		PrintDiffs(options);

		ionic::Table table(options);
		AddVar4Rows(table);
		table.print();
		std::cout << std::endl;
	}
	{
		ionic::TableOptions options;
		options.border = false;
		options.hDivider = false;
		options.vDivider = false;
		PrintDiffs(options);

		ionic::Table table(options);
		AddVar4Rows(table);
		table.print();
		std::cout << std::endl;
	}
}

int main()
{
	std::cout << "Welcome to ionic. (https://github.com/leethomason/ionic)\nA simple table formatter for console output in c++.\n\n";

	Print6();

	{
		ionic::TableOptions options;
		options.maxWidth = 50;
		ionic::Table t5(options);
		t5.setColumns({ {1},
						{4},
						{0},
						{0},
						{0} });
		t5.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
		t5.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
		t5.print();
		PrintRuler(options.maxWidth);
	}
	{
		ionic::TableOptions options;
		options.maxWidth = 80;
		ionic::Table t(options);
		t.setColumns({ {1}, {4}, {0}, {0}, {0} });
		t.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
		t.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
		t.print();
		PrintRuler(options.maxWidth);
	}
	{
		ionic::Table t6;
		t6.setColumns({ {1}, {4}, {0}, {0}, {0} });
		t6.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
		t6.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
		t6.print();
	}
	{
		ionic::TableOptions options;
		options.hChar = '=';
		options.vChar = 'I';
		options.cornerChar = 'O';
		ionic::Table t(options);;
		t.addRow({ "1", "4", "Dyn", "Dyn", "Dyn" });
		t.addRow({ "a", "TooLong", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Hello", "It was a bright cold day in April, and the clocks were striking thirteen." });
		t.print();
	}
	{
		ionic::TableOptions options;
		options.indent = 4;
		ionic::Table t(options);
		t.setColumns({ {15} });
		t.addRow({ "123456789012345" });
		t.addRow({ "It was a bright cold day in April, and the clocks were striking thirteen." });
		std::cout << t;
	}
	{
		ionic::TableOptions options;
		options.tableColor = ionic::Color::blue;
		ionic::Table t(options);

		t.setColumns({
			{0},
			{0, ionic::Color::red},
			{0, ionic::Color::green},
			{0, ionic::Color::blue},
			{0, ionic::Color::yellow},
			{0, ionic::Color::magenta},
			{0, ionic::Color::cyan} });
		t.addRow({ "", "R", "G", "B", "Y", "M", "C" });
		t.addRow({ "Normal", "Red", "Green", "Blue", "Yellow", "Magenta", "Cyan" });
		t.updateColumns({
			ionic::Color::white,
			ionic::Color::brightRed,
			ionic::Color::brightGreen,
			ionic::Color::brightBlue,
			ionic::Color::brightYellow,
			ionic::Color::brightMagenta,
			ionic::Color::brightCyan });
		t.addRow({ "Bright", "Red", "Green", "Blue", "Yellow", "Magenta", "Cyan" });

		t.print();
		std::cout << ionic::Table::colorize(ionic::Color::black, "This is Black text") << " (black)\n";
		std::cout << ionic::Table::colorize(ionic::Color::gray, "This is Dark Gray text") << " (dark gray)\n";
		std::cout << ionic::Table::colorize(ionic::Color::brightGray, "This is Gray text") << " (gray)\n";
		std::cout << ionic::Table::colorize(ionic::Color::white, "This is White text") << " (white)\n";
	}

	{
		ionic::Table t;
		t.setColumns({
			{10, ionic::Color::brightGreen, ionic::Alignment::left},
			{10, ionic::Color::yellow, ionic::Alignment::center},
			{10, ionic::Color::brightCyan, ionic::Alignment::right} });
		t.addRow({ { "This is left aligned text" }, { "This text is center aligned" }, { "And finally this is right aligned" } });
		t.print();
	}

	return 0;
}