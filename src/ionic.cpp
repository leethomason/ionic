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

void Ionic::addRow(const std::vector<std::string>& row)
{
	assert(row.size() == _cols.size());
	_rows.push_back(row);
}

void Ionic::print()
{
	std::vector<int> innerColWidth;
	innerColWidth.resize(_cols.size(), 0);

	for (size_t r = 0; r < _rows.size(); ++r) {
		for (size_t c = 0; c < _cols.size(); ++c) {
			if (_cols[c].type == ColType::kFixed) {
				innerColWidth[c] = std::max(innerColWidth[c], _cols[c].width);
			}
			else {
				innerColWidth[c] = std::max(innerColWidth[c], (int)_rows[r][c].size());
			}
		}
	}

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

			if (_cols[c].type == ColType::kFixed) {
				const std::string& s = _rows[r][c];
				if (s.size() <= _cols[c].width)
					fmt::print("{:<{}}", s, _cols[c].width);
				else
					fmt::print("{:<{}}", s.substr(0, _cols[c].width - 3) + "...", _cols[c].width);
			}
			else {
				fmt::print("{:<{}}", _rows[r][c], innerColWidth[c]);
			}

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

}  // namespace ionic