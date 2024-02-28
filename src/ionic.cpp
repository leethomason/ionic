#include "ionic/ionic.h"

#include <assert.h>
#include <algorithm>
#include <numeric>
#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlobj_core.h>
#endif

namespace ionic {															

void Table::initConsole()
{
#ifdef _WIN32
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD oldMode = 0;
	GetConsoleMode(handle, &oldMode);
	DWORD mode = oldMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(handle, mode);
#endif
}

int Table::terminalWidth() const
{
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
#	error "Not implemented"
#endif // _WIN32
}

#ifdef _WIN32
// Sigh.
#undef min
#undef max
#endif // _WIN32


void append(std::string& s, char a, char b) {
	s.push_back(a);
	s.push_back(b);
}

void append(std::string& s, char a, char b, char c) {
	s.push_back(a);
	s.push_back(b);
	s.push_back(c);
}

void append(std::string& s, const std::string& a, int width)
{
	s += a;
	if (a.size() < width) {
		s.append(width - a.size(), ' ');
	}
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
		cvec.resize(row.size(), Column{ ColType::kDynamic, 0 });
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
		if (c.type == ColType::kFixed) {
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
			if (_cols[i].type == ColType::kDynamic) {
				inner[i] = kMinWidth;
			}
		}
		return inner;
	}

	int avail = w - fixedWidth;
	int grant = avail / nDyn;

	std::vector<int> dynCols;
	for (size_t i = 0; i < _cols.size(); ++i) {
		if (_cols[i].type == ColType::kDynamic) {
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

/*static*/ Table::Break Table::lineBreak(const std::string& text, size_t start, size_t end, int width)
{
	// Don't think about newlines - they are handled by the caller.
	// (But do check we were called correctly.)
	assert(text.size() == end || text[end] == '\n');

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
	std::vector<Break> lines;
	size_t start = 0;

	while (start < text.size()) {
		// Newlines are hard breaks.
		size_t end = text.find('\n', start);
		end = std::min(end, text.size());

		// Hit a new line.
		if (end == start) {
			lines.push_back(Break{ start, start + 1, start + 1});
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
	std::cout << format();
}

std::string Table::format() const
{
	std::string out;
	if (_cols.empty() || _rows.empty()) {
		return out;
	}

	int outerWidth = _options.maxWidth > 0 ? _options.maxWidth : terminalWidth();
	int innerWidth = outerWidth;
	if (_options.outerBorder)
		innerWidth -= 2 * 2;	// 2 for each border
	innerWidth -= 3 * int(_cols.size() - 1);	// 3 for each inner border

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

	if (_options.outerBorder)
		printTBBorder(out, innerColWidth);

	for (size_t r = 0; r < _rows.size(); ++r) {
		std::vector<std::vector<Break>> breaks;
		breaks.resize(_cols.size());
		for (size_t c = 0; c < _cols.size(); ++c) {
			const std::string& s = _rows[r][c].text;
			breaks[c] = wordWrap(s, innerColWidth[c]);
		}

		bool done = false;
		int line = 0;
		while (!done) {
			done = true;
			if (_options.outerBorder) append(out, _options.borderVChar, ' ');

			for (size_t c = 0; c < _cols.size(); ++c) {
				if (c > 0)
					append(out, ' ', _options.borderVChar, ' ');

				std::string view;
				if (line < breaks[c].size()) {
					if (line < breaks[c].size() - 1)
						done = false;
					const std::string& str = _rows[r][c].text;
					view = str.substr(
						breaks[c][line].start,
						breaks[c][line].end - breaks[c][line].start);
				}

				int width = innerColWidth[c];
				if (view.size() <= width) {
					append(out, view, width);
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
			++line;
			if (_options.outerBorder)
				append(out, ' ', _options.borderVChar);
			out += '\n';
		}
		if (_options.innerHorizontalDivider && r < _rows.size() - 1)
			printTBBorder(out, innerColWidth);
	}

	if (_options.outerBorder)
		printTBBorder(out, innerColWidth);
	
	return out;
}


void Table::printTBBorder(std::string& s, const std::vector<int>& innerColWidth) const
{
	std::string buf;
	buf.clear();
	if (_options.outerBorder) {
		for (size_t c = 0; c < _cols.size(); ++c) {
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
	s += buf;
	s.push_back('\n');
}

#define TEST(x)                                                 \
	if (!(x)) {	                                                \
		printf("ERROR: line %d in %s\n", __LINE__, __FILE__);   \
        assert(false);                                          \
		return false;										    \
	}

/*static*/ bool Table::test()
{
	{
		std::string t0 = "This\r\nis multi-line\n\rstring\n\r  \n";
		normalizeNL(t0);
		TEST(t0.find('\r') == std::string::npos);
		TEST(!t0.empty());

		trimRight(t0);
		TEST(t0 == "This\nis multi-line\nstring");

		int max = 0;
		int nSub = nLines(t0, max);
		TEST(nSub == 3);
		TEST(max == 13);
	}
	{
		std::string t1 = "Hello";
		int max = 0;
		int nSub = nLines(t1, max);
		TEST(nSub == 1);
		TEST(max == 5);
	}
	{
		//                   0    5    10   15   20   25   30   35   40
		std::string line0 = "This is a test.";

		Break r = lineBreak(line0, 0, line0.size(), 15);
		TEST(r.start == 0);
		TEST(r.end == 15);
		TEST(r.next == 15);

		r = lineBreak(line0, 0, line0.size(), 100);
		TEST(r.start == 0);
		TEST(r.end == 15);
		TEST(r.next == 15);

		for (int s = 1; s < 6; s++) {
			r = lineBreak(line0, 0, line0.size(), s);
			TEST(r.start == 0 && r.end == 4 && r.next == 5);
		}
		r = lineBreak(line0, 0, line0.size(), 5);
		TEST(r.start == 0 && r.end == 4 && r.next == 5);
		for (int s = 7; s < 9; s++) {
			r = lineBreak(line0, 0, line0.size(), s);
			TEST(r.start == 0 && r.end == 7 && r.next == 8);
		}
		for (int s = 9; s < 15; s++) {
			r = lineBreak(line0, 0, line0.size(), s);
			TEST(r.start == 0 && r.end == 9 && r.next == 10);
		}

		r = lineBreak(line0, 0, line0.size(), 15);
		TEST(r.start == 0 && r.end == 15 && r.next == 15);
	}
	{
		//                   0    5
		std::string line1 = "Test  ";
		Break r = lineBreak(line1, 0, line1.size(), 100);
		TEST(r.start == 0);
		TEST(r.end == 4);
		TEST(r.next == 6);
	}
	{
		//                   0    5    10   15   20   25   30   35   40
		std::string line2 = "Prev. Test  ";
		Break r = lineBreak(line2, 6, line2.size(), 100);
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
		std::vector<Break> breaks = wordWrap(line3, 15);
		TEST(breaks.size() == 6);
		TEST(breaks[0].start == 0  && breaks[0].end == 15 && breaks[0].next == 16);
		TEST(breaks[1].start == 16 && breaks[1].end == 27 && breaks[1].next == 28);
		TEST(breaks[2].start == 28 && breaks[2].end == 42 && breaks[2].next == 43);
		TEST(breaks[3].start == 43 && breaks[3].end == 54 && breaks[3].next == 55);
		TEST(breaks[4].start == 55 && breaks[4].end == 63 && breaks[4].next == 64);
		TEST(breaks[5].start == 64 && breaks[5].end == 73 && breaks[5].next == 73);
	}
	{
		//                   0    5 _ _  10   15   20_   25   30   35 _  40   45   50   55   60   65   70   75   80   85   90   95   100
		std::string line4 = "A Poem.\n\nTo challenge\nthe line breaker\n";
		std::vector<Break> breaks = wordWrap(line4, 15);
		TEST(breaks.size() == 5);
		TEST(breaks[0].start == 0  && breaks[0].end == 7 && breaks[0].next == 8);
		TEST(breaks[1].start == 8  && breaks[1].end == 9 && breaks[1].next == 9);
		TEST(breaks[2].start == 9  && breaks[2].end == 21 && breaks[2].next == 22);
		TEST(breaks[3].start == 22 && breaks[3].end == 30 && breaks[3].next == 31);
		TEST(breaks[4].start == 31 && breaks[4].end == 38 && breaks[4].next == 39);
	}
	return true;
}


}  // namespace ionic