#include "ionic/ionic.h"

#include <fmt/core.h>

#include <assert.h>

namespace ionic {															

void Ionic::initConsole()
{
	// FIXME
}

void Ionic::setColumnFormat(const std::vector<Ionic::Column>& cols)
{
	_cols = cols;
}

/*static*/ int Ionic::nSubStrings(const std::string& s, int& maxWidth)
{
	int n = 0;
	maxWidth = 0;
	size_t pos = 0;

	while (pos < s.size()) {
		size_t next = s.find('\n', pos);
		if (next == std::string::npos) {
			next = s.size();
		}
		n++;
		size_t w = next - pos;
		maxWidth = std::max(maxWidth, (int)w);
		pos = next + 1;
	}
	return n;
}


void Ionic::addRow(const std::vector<std::string>& row)
{
	if (_cols.empty()) {
		std::vector<Ionic::Column> cvec;
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

		int mw = 0;
		int nSub = nSubStrings(c.text, mw);
		c.wrap = nSub > 1;
		c.desiredWidth = mw;
	}
}

void Ionic::computeWidths(std::vector<int>& innerColWidth)
{
	innerColWidth.resize(_cols.size(), 0);

	for (size_t r = 0; r < _rows.size(); ++r) {
		for (size_t c = 0; c < _cols.size(); ++c) {
			//if (_cols[c].type == ColType::kFixed) {
			//	innerColWidth[c] = std::max(innerColWidth[c], _cols[c].width);
			//}
			//else {
			//	innerColWidth[c] = std::max(innerColWidth[c], (int)_rows[r][c].size());
			//}
		}
	}
	int w = maxWidth;
	if (w < 0) {
		w = terminalWidth();
	}
	int innerWidth = w;
	if (outerBorder)
		innerWidth -= 4;
	innerWidth -= 3 * int(_cols.size() - 1);

	int nWrap = 0;
	for (size_t c = 0; c < _cols.size(); ++c) {
		//if (_cols[c].type == ColType::kWrap) {
		//	++nWrap;
		//}
		//else {
			innerWidth -= innerColWidth[c];
		//}
	}
	/*
	if (innerWidth > 0 && nWrap > 0) {
		int wrapWidth = innerWidth / nWrap;
		for (size_t c = 0; c < _cols.size(); ++c) {
			if (_cols[c].type == ColType::kWrap) {
				if (nWrap > 1) {
					innerColWidth[c] = wrapWidth;
					nWrap--;
					innerWidth -= wrapWidth;
				}
				else {
					innerColWidth[c] = innerWidth;
				}
			}
		}
	}
	*/
}

void Ionic::print()
{
	std::vector<int> innerColWidth;
	computeWidths(innerColWidth);

	/*
		Fixed(1), Dynamic, Wrap
		Inner
		  1   5       15
		Outer
		 4,2  8,2    17,2               extra x
		+---+-------+-----------------+
	    | 0 | A     | The Outer World |
		+---+-------+-----------------+
		| 1 | Hello | And Another     |
		+---+-------+-----------------+ extra y
	
	*/
	if (outerBorder)
		printTBBorder(innerColWidth);

	for (size_t r = 0; r < _rows.size(); ++r) {
		if (outerBorder)
			fmt::print("{} ", borderVChar);

		for (size_t c = 0; c < _cols.size(); ++c) {
			if (c > 0)
				fmt::print(" {} ", borderVChar);

			const std::string& s = _rows[r][c].text;
			int width = innerColWidth[c];
			if (s.size() <= width)
				fmt::print("{:<{}}", s, width);
			else
				fmt::print("{:<}", s.substr(0, width));
		}
		if (outerBorder)
			fmt::print(" {}", borderVChar);
		fmt::print("\n");
		if (innerHorizontalDivider && r < _rows.size() - 1)
			printTBBorder(innerColWidth);
	}

	if (outerBorder)
		printTBBorder(innerColWidth);
}


void Ionic::printTBBorder(const std::vector<int>& innerColWidth)
{
	_buf.clear();
	if (outerBorder) {
		for (size_t c = 0; c < _cols.size(); ++c) {
			_buf += borderCornerChar;
			_buf.append(2 + innerColWidth[c], borderHChar);
		}
		_buf += borderCornerChar;
	}
	else {
		_buf.append(1 + innerColWidth[0], borderHChar);
		for (size_t c = 1; c < _cols.size(); ++c) {
			_buf += borderCornerChar;
			_buf.append(2 + innerColWidth[c], borderHChar);
		}
	}
	fmt::print("{}\n", _buf);
}

#define TEST(x)                                                 \
	if (!(x)) {	                                                \
		printf("ERROR: line %d in %s\n", __LINE__, __FILE__);   \
        assert(false);                                          \
		return false;										    \
	}

/*static*/ bool Ionic::test()
{
	std::string t0 = "This\r\nis multi-line\n\rstring\n\r  \n";
	normalizeNL(t0);
	TEST(t0.find('\r') == std::string::npos);
	TEST(!t0.empty());

	trimRight(t0);
	TEST(t0 == "This\nis multi-line\nstring");

	int max = 0;
	int nSub = nSubStrings(t0, max);
	TEST(nSub == 3);
	TEST(max == 13);

	std::string t1 = "Hello";
	nSub = nSubStrings(t1, max);
	TEST(nSub == 1);
	TEST(max == 5);

	return true;
}


}  // namespace ionic