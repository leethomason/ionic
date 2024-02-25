#include "ionic/ionic.h"

#include <fmt/core.h>

#include <assert.h>
#include <algorithm>
#include <numeric>

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

		c.nLines = nSubStrings(c.text, c.desiredWidth);
	}
	_rows.push_back(r);
}

std::vector<int> Ionic::computeWidths(int w) const
{
	std::vector<int> inner(_cols.size(), 0);

	for (size_t i = 0; i < _cols.size(); ++i) {
		const Column& c = _cols[i];
		if (c.type == ColType::kFixed) {
			inner[i] = c.requestedWidth;
		}
		else {
			for (size_t j = 0; j < _rows.size(); ++j) {
				inner[i] = std::max(inner[i], _rows[j][i].desiredWidth);
			}
		}
	}
	if (std::accumulate(inner.begin(), inner.end(), 0) <= w) {
		return inner; // enough space - no allocation needed
	}

	// Extract the dynamic columns, remove the fixed ones, and compute the space available.
	struct ColInfo {
		int index;
		int width;
	};
	std::vector<ColInfo> dynCols;
	int avail = w;
	for (size_t i = 0; i < _cols.size(); ++i) {
		const Column& c = _cols[i];
		if (c.type == ColType::kFixed) {
			avail -= c.requestedWidth;
		}
		else {
			dynCols.push_back({ (int)i, inner[i] });
		}
	}
	if (avail <= dynCols.size() * kMinWidth) {
		// No space. We are out of luck.
		for (const auto& ci : dynCols)
			inner[ci.index] = kMinWidth;
		return inner;
	}
	int grant = avail / (int)dynCols.size();
	assert(grant >= kMinWidth);

	// Any column that is less the grant can be removed from the pool.
	dynCols.erase(std::remove_if(dynCols.begin(), dynCols.end(), 
		[grant](const ColInfo& ci) { return ci.width < grant; }), dynCols.end());

	if (dynCols.empty())
		return inner;

	// recompute grant
	grant = avail / (int)dynCols.size();

	for (size_t i = 0; i < dynCols.size(); ++i) {
		if (i == dynCols.size() - 1) {
			inner[dynCols[i].index] = avail;
		}
		else {
			if (inner[dynCols[i].index] < grant) {
				avail -= inner[dynCols[i].index];
			}
			else {
				inner[dynCols[i].index] = grant;
				avail -= grant;
			}

		}
	}
	return inner;
}

void Ionic::print()
{
	if (_cols.empty() || _rows.empty()) {
		return;
	}

	int outerWidth = maxWidth > 0 ? maxWidth : terminalWidth();
	int innerWidth = outerWidth;
	if (outerBorder)
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