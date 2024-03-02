# Ionic

Simple tables for console output for C++.

![Ionic Example](example.png)

Currently Windows, but Linux and OSX coming soon.

Ionic is a simple library for outputting tables to the console. It can print
in color, line break, align text, and supports several output styles.
Ionic is simple; it doesn't have a lot of features, but it's easy to use, and
runs reasonably efficiently.

Ionic is similar to Tabulate (https://github.com/p-ranav/tabulate) but it is
much simpler, both in code and features.

It has no dependencies beyond C++ 17. Ionic does have platform code - it calls
windows / linux / osx functions to initialize the console and query size.

## Install and Build

Ionic is one header and one cpp file. The best way to use it is cmake's
fetch_content. Here's an example:

```cmake
include(FetchContent)
...
FetchContent_Declare(
  ionic
  GIT_REPOSITORY git@github.com:leethomason/ionic.git
  GIT_TAG        <current git commit hash>
)
FetchContent_MakeAvailable(ionic)
```

Regrettably there is a bug in the ionic makefile, so you will also need to add 
the souce files to your project. Here's an example:

```cmake
add_executable(MyExample simple_example.cpp ${ionic_SOURCE_DIR}/ionic.cpp)
```

Which is very annoying, and I'd appreciate a PR to fix it. (I'll fix it as soon
as I understand cmake well enough.)

You can also just copy the two files (ionic.h and ionic.cpp) into your project.

### Versioning

There is no versioning and won't be. On other projects I've found versioning
and the infrastructure around it was more effort than the code itself. This is 
a very simple code base, will stay simple, and you should simple get the
version you want from the git tag.

Main is kept stable with automated tests.

## Usage

1. Construct a Table with TableOptions. (See TableOptions for features that can be set.)

```c++
        ionic::TableOptions options;
        options.outerBorder = true;
        options.innerHDivider = false;

        ionic::Table table(options);
```

2. Optional: Set the column format with setColumnFormat(). You can specify columns to be
   fixed width or flex width. If you don't setColumFormat(), all columns will be flex.

```c++
        table.setColumnFormat({
            {ionic::ColType::fixed, 4},
            {ionic::ColType::flex} });
```
    Note when you specify a column width, it is the width of the text, not the width of
    the column with padding and whitespace and such.


3. Add text rows with `addRow()`. The number of columns in `addRow()` calls and 
   `setColumnFormat()` calls must match.

4. Optional: Set the color and alignment of individual cells, rows, columns, or the entire table. 
   Use `setCell()`, `setRow()`, `setColumn()`, and `setTable()`.

5. Call `format()` to get the formatted table as a string, or `print()` to print it to the console or use the `<<` operator to print it to an ostream.

Enjoy! I hope you find it useful.

