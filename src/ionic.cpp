#include "ionic/ionic.h"

#include <assert.h>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <atomic>

#if defined(_WIN32)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	include <shlobj_core.h>
#elif __linux__
#	include <sys/ioctl.h>
#	include <stdio.h>
#	include <unistd.h>
#elif __APPLE__
#    include <sys/ioctl.h>
#    include <stdio.h>
#    include <unistd.h>
#else
#	error "undefined"
#endif

namespace ionic {															

bool Table::colorEnabled = true;

void Table::initConsole()
{
	static std::atomic<bool> init = false;
	if (!init.exchange(true)) {
#ifdef _WIN32
		// Win10 setup code, and I did need it there. Now...I have no
		// Win10 machine and nothing to test this one, so it's questionable
		// whether I should keep it.
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD oldMode = 0;
		GetConsoleMode(handle, &oldMode);
		DWORD mode = oldMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(handle, mode);
#endif
	}
}

int Table::consoleWidth() 
{
	initConsole();
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	int w = csbi.srWindow.Right - csbi.srWindow.Left + 1;

	// Git bash on Windows doesn't report the correct width.
	//_dupenv_s(&s, 0, "COLUMNS"); // this doesn't work, because it's not an environment var. (It works on the command line.)
	// I don't want to write a text file and incur issues there. 
	// popen() fires up a shell, which is absurd.
	// Looking for a better way.
	// For now it's hardcoded.
	if (w < 4) {
		w = 80;
	}
	return w;
#elif  __APPLE__
    struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
#elif __linux__
	struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
#else
#	error "Not implemented"
#endif // _WIN32
}

#ifdef _WIN32
// Sigh.
#undef min
#undef max
#endif // _WIN32


std::string colorCode(Color c)
{
	switch (c) {
	case Color::black: return "\x1B[30m";
	case Color::red: return "\x1B[31m";
	case Color::green: return "\x1B[32m";
	case Color::yellow: return "\x1B[33m";
	case Color::blue: return "\x1B[34m";
	case Color::magenta: return "\x1B[35m";
	case Color::cyan: return "\x1B[36m";
	
	case Color::brightGray: return "\x1B[37m";    // the brighter gray is here?
	case Color::gray: return "\x1B[90m";	// and this is the dark gray?

	case Color::brightRed: return "\x1B[91m";
	case Color::brightGreen: return "\x1B[92m";
	case Color::brightYellow: return "\x1B[93m";
	case Color::brightBlue: return "\x1B[94m";
	case Color::brightMagenta: return "\x1B[95m";
	case Color::brightCyan: return "\x1B[96m";
	case Color::white: return "\x1B[97m";

	case Color::kDefault: return "\033[0m";	// the reset value
	case Color::reset: return "\033[0m"; // reset again
	}
	return "";
}

std::string colorToStr(Color color)
{
	switch (color) {
	case Color::black: return "black";
	case Color::red: return "red";
	case Color::green: return "green";
	case Color::yellow: return "yellow";
	case Color::blue: return "blue";
	case Color::magenta: return "magenta";
	case Color::cyan: return "cyan";
	case Color::brightGray: return "brightGray";
	case Color::gray: return "gray";
	case Color::brightRed: return "brightRed";
	case Color::brightGreen: return "brightGreen";
	case Color::brightYellow: return "brightYellow";
	case Color::brightBlue: return "brightBlue";
	case Color::brightMagenta: return "brightMagenta";
	case Color::brightCyan: return "brightCyan";
	case Color::white: return "white";
	case Color::kDefault: return "default";
	case Color::reset: return "reset";
	}
	return "reset";
}

Color strToColor(const std::string& _str)
{
	// pull out spaces 
	std::string str;
	for (char s : _str) {
		if (s == '_' || s == '-' || std::isspace(s))
			continue;
		str += char(std::tolower(s));
	}

	bool dark = true;
	// c++ 17: don't have starts_with()
	const std::string LIGHT = "light";
	const std::string BRIGHT = "bright";
	const std::string PALE = "pale";
	if (str.compare(0, LIGHT.size(), LIGHT) == 0) {
		dark = false;
		str = str.substr(LIGHT.size());
	}
	else if (str.compare(0, BRIGHT.size(), BRIGHT) == 0) {
		dark = false;
		str = str.substr(BRIGHT.size());
	}
	else if (str.compare(0, PALE.size(), PALE) == 0) {
		dark = false;
		str = str.substr(PALE.size());
	}

	const std::string DARK = "dark";
	if (str.compare(0, DARK.size(), DARK) == 0) {
		str = str.substr(DARK.size());
	}

	if (str == "black") return Color::black;
	if (str == "white") return Color::white;
	if (str == "gray" || str == "grey") return dark ? Color::gray : Color::brightGray;

	if (str == "red") return dark ? Color::red : Color::brightRed;
	if (str == "green") return dark ? Color::green : Color::brightGreen;
	if (str == "yellow") return dark ? Color::yellow : Color::brightYellow;
	if (str == "blue") return dark ? Color::blue : Color::brightBlue;
	if (str == "magenta") return dark ? Color::magenta : Color::brightMagenta;
	if (str == "cyan") return dark ? Color::cyan : Color::brightCyan;

	return Color::reset;
}

Table::Dye::Dye(Color c, std::string& s) : _c(c), _s(s)
{
	if (_c != Color::kDefault && Table::colorEnabled)
		_s += colorCode(c);
}

Table::Dye::~Dye() {
	if (_c != Color::kDefault && Table::colorEnabled)
		_s += colorCode(Color::reset);
}

void append(std::string& s, char a, char b) 
{
	s.push_back(a);
	s.push_back(b);
}

void append(std::string& s, char a, char b, char c) 
{
	s.push_back(a);
	s.push_back(b);
	s.push_back(c);
}

void Table::printLeft(std::string& s) const
{
	if (_options.outerBorder) {
		Dye dye(_options.tableColor, s);
		append(s, _options.borderVChar, ' ');
	}
}

void Table::printRight(std::string& s) const
{
	if (_options.outerBorder) {
		Dye dye(_options.tableColor, s);
		append(s, ' ', _options.borderVChar);
	}
}

void Table::printCenter(std::string& s) const
{
	Dye dye(_options.tableColor, s);
	if (_options.innerVDivider)
		append(s, ' ', _options.borderVChar, ' ');
	else
		append(s, ' ', ' ');
}

void Table::setColumnFormat(const std::vector<Table::Column>& cols)
{
	_cols = cols;
}

/*static*/ int Table::nLines(const std::string& s, int& maxWidth)
{
	int n = 0;
	maxWidth = 0;
	size_t pos = 0;

	while (pos < s.size()) {
		size_t next = s.find('\n', pos);
		next = std::min(next, s.size());

		n++;
		size_t w = next - pos;
		maxWidth = std::max(maxWidth, (int)w);
		pos = next + 1;
	}
	return n;
}

void Table::addRow(const std::vector<std::string>& row)
{
	if (_cols.empty()) {
		std::vector<Table::Column> cvec;
		cvec.resize(row.size(), Column{ ColType::flex, 0 });
		setColumnFormat(cvec);
	}
	assert(row.size() == _cols.size());
	
	std::vector<Cell> r(row.size());
	for(size_t i=0; i<row.size(); ++i) {
		Cell& c = r[i];
		c.text = row[i];
		normalizeNL(c.text);
		trimRight(c.text);		// right trailing spaces are presumably extraneous

		c.nLines = nLines(c.text, c.desiredWidth);
		c.color = _options.textColor;
		c.alignment = _options.alignment;
	}
	_rows.push_back(r);
}

void Table::setCell(int row, int col, std::optional<Color> color, std::optional<Alignment> alignment)
{
	Cell& cell = _rows[row][col];
	if (color) {
		cell.color = *color;
	}
	if (alignment) {
		cell.alignment = *alignment;
	}
}

void Table::setRow(int row, std::optional<Color> color, std::optional<Alignment> alignment)
{
	for (size_t c = 0; c < _rows[row].size(); ++c) {
		setCell(row, int(c), color, alignment);
	}
}

void Table::setColumn(int col, std::optional<Color> color, std::optional<Alignment> alignment)
{
	for (size_t r = 0; r < _rows.size(); ++r) {
		setCell(int(r), col, color, alignment);
	}
}

void Table::setTable(std::optional<Color> color, std::optional<Alignment> alignment)
{
	for (size_t r = 0; r < _rows.size(); ++r) {
		setRow(int(r), color, alignment);
	}
}

std::vector<int> Table::computeWidths(const int w) const
{
	std::vector<int> inner(_cols.size(), 0);

	int requiredWidth = 0;
	int fixedWidth = 0;
	int nDyn = 0;

	for (size_t i = 0; i < _cols.size(); ++i) {
		const Column& c = _cols[i];
		if (c.type == ColType::fixed) {
			inner[i] = c.requestedWidth;
			requiredWidth += c.requestedWidth;
			fixedWidth += c.requestedWidth;
		}
		else {
			for (size_t j = 0; j < _rows.size(); ++j) {
				inner[i] = std::max(inner[i], _rows[j][i].desiredWidth);
			}
			requiredWidth += kMinWidth;
			++nDyn;
		}
	}
	if (std::accumulate(inner.begin(), inner.end(), 0) <= w) {
		return inner; // enough space - no allocation needed
	}

	if (requiredWidth >= w) {
		// Nothing we can do.
		for (size_t i = 0; i < _cols.size(); ++i) {
			if (_cols[i].type == ColType::flex) {
				inner[i] = kMinWidth;
			}
		}
		return inner;
	}

	int avail = w - fixedWidth;
	int grant = avail / nDyn;

	std::vector<int> dynCols;
	for (size_t i = 0; i < _cols.size(); ++i) {
		if (_cols[i].type == ColType::flex) {
			if (inner[i] <= grant) {
				avail -= inner[i];
			}
			else {
				dynCols.push_back((int)i);
			}
		}
	}

	if (dynCols.empty()) {
		assert(std::accumulate(inner.begin(), inner.end(), 0) <= w);
		return inner;
	}

	assert(dynCols.size());
	int grant2 = avail / int(dynCols.size());
	for (size_t i = 0; i < dynCols.size() - 1; ++i) {
		if (grant2 >= inner[dynCols[i]]) {
			avail -= inner[dynCols[i]];
		}
		else {
			inner[dynCols[i]] = grant2;
			avail -= grant2;
		}
	}
	inner[dynCols.back()] = avail;

	assert(std::accumulate(inner.begin(), inner.end(), 0) == w);
	return inner;
}

/*static*/ Table::Break Table::lineBreak(const std::string& text, size_t start, size_t end, int p_width)
{
	// Don't think about newlines - they are handled by the caller.
	// (But do check we were called correctly.)
	assert(text.size() == end || text[end] == '\n');
	assert(p_width > 0);
	const size_t width = (size_t)p_width;

	size_t pos = start;
	size_t nextSpace = start;
	size_t prevSpace = start;
	size_t next = start;
	size_t prev = start;

	while (next < end) {
		nextSpace = text.find_first_of(kSpace, pos);
		nextSpace = std::min(nextSpace, end);
		next = text.find_first_not_of(kSpace, nextSpace + 1);
		next = std::min(next, end);

		assert(nextSpace == end || nextSpace < next);

		if (nextSpace - start > width) {
			if (prev == start) {
				return Break{ start, nextSpace, next };	// truncate words greater than column width
			}
			else {
				return Break{ start, prevSpace, prev };
			}
		}
		pos = next;
		prev = next;
		prevSpace = nextSpace;
	}
	return Break{ start, nextSpace, next };
}

/*static*/ std::vector<Table::Break> Table::wordWrap(const std::string& text, int width)
{
	if (width == 0)
		width = consoleWidth();

	std::vector<Break> lines;
	size_t start = 0;

	while (start < text.size()) {
		// Newlines are hard breaks.
		size_t end = text.find('\n', start);
		end = std::min(end, text.size());

		// Hit a new line.
		if (end == start) {
			lines.push_back(Break{ start, start, start + 1});
			start = end + 1;
			continue;
		}

		Break bk = lineBreak(text, start, end, width);
		if (bk.next < text.size() && text[bk.next] == '\n') {
			bk.next++;
		}
		lines.push_back(bk);

		start = bk.next;
	}
	return lines;
}

void Table::print() const
{
	initConsole();
	std::cout << format();
}

std::string Table::format() const
{
	std::string out;
	if (_cols.empty() || _rows.empty()) {
		return out;
	}

	int vDivWidth = _options.innerVDivider ? 3 : 2;

	int outerWidth = _options.maxWidth > 0 ? _options.maxWidth : consoleWidth();
	int innerWidth = outerWidth - _options.indent;
	if (_options.outerBorder)
		innerWidth -= 2 * 2;	// 2 for each border
	innerWidth -= vDivWidth * (int(_cols.size()) - 1);	// 3 for each inner border

	std::vector<int> innerColWidth = computeWidths(innerWidth);
	
	/*
		Fixed(1), Dynamic, Wrap
		Inner
		  1   5       15
		+---+-------+-----------------+
	    | 0 | A     | The Outer World |
		+---+-------+-----------------+
		| 1 | Hello | And Another     |
		+---+-------+-----------------+ extra y
	
	*/

	out.reserve(outerWidth * _rows.size() * 2);	// rough guess

	printHorizontalBorder(out, innerColWidth, true);

	for (size_t r = 0; r < _rows.size(); ++r) {
		std::vector<std::vector<Break>> breaks;
		breaks.resize(_cols.size());
		for (size_t c = 0; c < _cols.size(); ++c) {
			const std::string& s = _rows[r][c].text;
			breaks[c] = wordWrap(s, innerColWidth[c]);
		}

		bool done = false;
		size_t line = 0;
		while (!done) {
			done = true;
			out.append(_options.indent, ' ');
			printLeft(out);

			for (size_t c = 0; c < _cols.size(); ++c) {
				if (c > 0)
					printCenter(out);

				std::string view;
				if (line < breaks[c].size()) {
					if (line + 1 < breaks[c].size())
						done = false;
					const std::string& str = _rows[r][c].text;
					view = str.substr(
						breaks[c][line].start,
						breaks[c][line].end - breaks[c][line].start);
				}

				assert(innerColWidth[c] >= 0);
				size_t width = innerColWidth[c];
				{
					Dye dye(_rows[r][c].color, out);
					Alignment align = _rows[r][c].alignment;

					if (view.size() <= width) {
						// It's only where the text fits that the alignment matters.
						if (align == Alignment::left) {
							out += view;
							out.append(width - view.size(), ' ');
						}
						else if (align == Alignment::right) {
							out.append(width - view.size(), ' ');
							out += view;
						}
						else if (align == Alignment::center) {
							int left = int(width - view.size()) / 2;
							out.append(left, ' ');
							out += view;
							out.append(width - left - view.size(), ' ');
						}
					}
					else {
						const std::string ellipsis = kEllipsis;
						if (width <= ellipsis.size()) {
							out += ellipsis.substr(0, width);
						}
						else {
							out += view.substr(0, width - ellipsis.size());
							out += ellipsis;
						}
					}
				}
			}
			++line;
			printRight(out);
			out += '\n';
		}
		if (r + 1 < _rows.size()) {
			printHorizontalBorder(out, innerColWidth, false);
		}
	}

	printHorizontalBorder(out, innerColWidth, true);
	
	return out;
}


void Table::printHorizontalBorder(std::string& s, const std::vector<int>& innerColWidth, bool outer) const
{
	if (outer && !_options.outerBorder)
		return;
	if (!outer && !_options.innerHDivider)
		return;

	std::string buf;

	{
		buf.append(_options.indent, ' ');
		Dye dye(_options.tableColor, buf);
		if (_options.outerBorder) {
			for (size_t c = 0; c < _cols.size(); ++c) {
				if (c == 0 || _options.innerVDivider)
					buf += _options.borderCornerChar;
				buf.append(2 + innerColWidth[c], _options.borderHChar);
			}
			buf += _options.borderCornerChar;
		}
		else {
			buf.append(1 + innerColWidth[0], _options.borderHChar);
			for (size_t c = 1; c < _cols.size(); ++c) {
				buf += _options.borderCornerChar;
				buf.append(2 + innerColWidth[c], _options.borderHChar);
			}
		}
	}
	s += buf;
	s.push_back('\n');
}

/*static*/ std::string Table::colorize(Color c, const std::string& s)
{
	if (c == Color::reset || !colorEnabled)
		return s;

	std::string in = colorCode(c);
	std::string out = colorCode(Color::reset);

	return in + s + out;
}

}  // namespace ionic
