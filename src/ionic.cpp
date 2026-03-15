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
    return w.ws_col > 0 ? w.ws_col : 80;
#elif __linux__
	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col > 0 ? w.ws_col : 80;
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
	if (_c != Color::reset && Table::colorEnabled)
		_s += colorCode(c);
}

Table::Dye::~Dye() {
	if (_c != Color::reset && Table::colorEnabled)
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
	if (_options.border) {
		Dye dye(_options.tableColor, s);
		append(s, _options.vChar, ' ');
	}
}

void Table::printRight(std::string& s) const
{
	if (_options.border) {
		Dye dye(_options.tableColor, s);
		append(s, ' ', _options.vChar);
	}
}

void Table::printCenter(std::string& s) const
{
	Dye dye(_options.tableColor, s);
	if (_options.vDivider)
		append(s, ' ', _options.vChar, ' ');
	else
		append(s, ' ', ' ');
}

void Table::setColumns(const std::vector<Column>& cols)
{
	if (_cols.empty()) {
		_cols = cols;
		return;
	}
	assert(cols.size() == _cols.size());
	for (size_t i = 0; i < _cols.size(); ++i) {
		_cols[i] = cols[i];
	}
}

void Table::updateColumns(const std::vector<Column>& formats)
{
	assert(_cols.size() == formats.size());
	for (size_t i = 0; i < _cols.size(); ++i) {
		_cols[i].color = formats[i].color;
		_cols[i].alignment = formats[i].alignment;
	}
}

void Table::updateColumns(const std::vector<Color>& colors)
{
	assert(_cols.size() == colors.size());
	for (size_t i = 0; i < _cols.size(); ++i) {
		_cols[i].color = colors[i];
	}
}

void Table::resetColumns()
{
	for (size_t i = 0; i < _cols.size(); ++i) {
		_cols[i] = Column{ _cols[i].width };
	}
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

void Table::addRow(const std::vector<std::string>& row, std::optional<Color> color)
{
	if (_cols.empty()) {
		std::vector<Column> cvec;
		cvec.resize(row.size(), Column{ 0 });
		setColumns(cvec);
	}
	assert(row.size() == _cols.size());
	
	std::vector<Cell> r(row.size());
	for(size_t i=0; i<row.size(); ++i) {
		Cell& c = r[i];
		c.text = row[i];
		normalizeNL(c.text);
		trimRight(c.text);		// right trailing spaces are presumably extraneous

		c.nLines = nLines(c.text, c.desiredWidth);
		c.color = color ? *color : (_cols[i].color ? *_cols[i].color : _options.textColor);
		c.alignment = _cols[i].alignment ? *_cols[i].alignment : _options.alignment;
	}
	_rows.push_back(r);
}

std::vector<int> Table::computeWidths(const int w) const
{
	std::vector<int> inner(_cols.size(), 0);

	int requiredWidth = 0;
	int fixedWidth = 0;
	int nDyn = 0;

	for (size_t i = 0; i < _cols.size(); ++i) {
		const Column& c = _cols[i];
		if (c.width > 0) {
			inner[i] = c.width;
			requiredWidth += c.width;
			fixedWidth += c.width;
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
			if (_cols[i].width == 0) {
				inner[i] = kMinWidth;
			}
		}
		return inner;
	}

	int avail = w - fixedWidth;
	int grant = avail / nDyn;

	std::vector<int> dynCols;
	for (size_t i = 0; i < _cols.size(); ++i) {
		if (_cols[i].width == 0) {
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

std::vector<int> Table::getInnerColWidths() const
{
	int vDivWidth = _options.vDivider ? 3 : 2;
	int outerWidth = _options.maxWidth > 0 ? _options.maxWidth : consoleWidth();
	int innerWidth = outerWidth - _options.indent;
	if (_options.border)
		innerWidth -= 2 * 2;	// 2 for each border
	innerWidth -= vDivWidth * (int(_cols.size()) - 1);
	return computeWidths(innerWidth);
}

std::string Table::formatRow(size_t r, const std::vector<int>& innerColWidth) const
{
	std::string out;

	std::vector<std::vector<Break>> breaks;
	breaks.resize(_cols.size());
	for (size_t c = 0; c < _cols.size(); ++c) {
		breaks[c] = wordWrap(_rows[r][c].text, innerColWidth[c]);
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
	return out;
}

std::string Table::format() const
{
	std::string out;
	if (_cols.empty() || _rows.empty()) {
		return out;
	}

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

	std::vector<int> innerColWidth = getInnerColWidths();
	int outerWidth = _options.maxWidth > 0 ? _options.maxWidth : consoleWidth();
	out.reserve(outerWidth * _rows.size() * 2);	// rough guess

	printHorizontalBorder(out, innerColWidth, true);

	for (size_t r = 0; r < _rows.size(); ++r) {
		out += formatRow(r, innerColWidth);
		if (r + 1 < _rows.size()) {
			printHorizontalBorder(out, innerColWidth, false);
		}
	}

	printHorizontalBorder(out, innerColWidth, true);

	return out;
}

std::vector<std::string> Table::formatRows() const
{
	std::vector<std::string> result;
	if (_cols.empty() || _rows.empty()) {
		return result;
	}

	std::vector<int> innerColWidth = getInnerColWidths();
	result.reserve(_rows.size());
	for (size_t r = 0; r < _rows.size(); ++r) {
		std::string s;
		// First row gets the top (outer) border; subsequent rows get the inner separator.
		printHorizontalBorder(s, innerColWidth, r == 0);
		s += formatRow(r, innerColWidth);
		// Last row gets the bottom (outer) border.
		if (r + 1 == _rows.size()) {
			printHorizontalBorder(s, innerColWidth, true);
		}
		result.push_back(std::move(s));
	}
	return result;
}


void Table::printHorizontalBorder(std::string& s, const std::vector<int>& innerColWidth, bool outer) const
{
	if (outer && !_options.border)
		return;
	if (!outer && !_options.hDivider)
		return;

	std::string buf;

	{
		buf.append(_options.indent, ' ');
		Dye dye(_options.tableColor, buf);
		if (_options.border) {
			for (size_t c = 0; c < _cols.size(); ++c) {
				if (c == 0 || _options.vDivider)
					buf += _options.cornerChar;
				buf.append(2 + innerColWidth[c], _options.hChar);
			}
			buf += _options.cornerChar;
		}
		else {
			buf.append(1 + innerColWidth[0], _options.hChar);
			for (size_t c = 1; c < _cols.size(); ++c) {
				buf += _options.cornerChar;
				buf.append(2 + innerColWidth[c], _options.hChar);
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
