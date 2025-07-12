# Ionic

A C++ library for outputting tables to the console.

[![Build and Test](https://github.com/leethomason/ionic/actions/workflows/test.yml/badge.svg?branch=main)](https://github.com/leethomason/ionic/actions/workflows/test.yml)

![Ionic Example](example.png)

Tested on Windows, Linux, and OSX.

Ionic is a simple library for outputting tables to the console. It can print
in color, line break, align text, and supports several output styles.
Ionic is simple; it doesn't have a lot of features, but it's easy to use, and
runs reasonably efficiently.

Ionic is similar to Tabulate (<https://github.com/p-ranav/tabulate>) but it is
much simpler, both in code and features. Ionic has built-in support for
terminal width, which is a convenience Tabulate does not have.

It has no dependencies beyond C++ 17. Ionic does have platform code - it calls
windows / linux / osx functions to initialize the console and query size.

## Install and Build

Ionic is one header and one cpp file. The best way to use it is CMake's
fetch_content. Here's an example:

```cmake
include(FetchContent)
...
FetchContent_Declare(
  ionic
  GIT_REPOSITORY https://github.com/leethomason/ionic.git
  GIT_TAG        <current git commit hash>
)
FetchContent_MakeAvailable(ionic)
```

Then in your project, you can link to the ionic library:

```cmake
add_executable(MyExample simple_example.cpp)
target_link_libraries(MyExample ionic::ionic)
```

You can also just copy the two files (ionic.h and ionic.cpp) into your project.

### Versioning

There is no versioning and won't be. On other projects I've found versioning
and the infrastructure around it was more effort than the code itself. This is
a very simple code base, will stay simple, and you can get the
version you want from the git tag.

Main is kept stable with automated tests.

## Usage

### Creating Tables

1. Construct a Table with TableOptions. (See TableOptions for features that can be set.)

```c++
        ionic::TableOptions options;
        options.outerBorder = true;
        options.innerHDivider = false;

        ionic::Table table(options);
```

2. Optional: Set the column format with setColumnFormat(). You can specify columns to be
   fixed width or flex width. If you don't setColumnFormat(), all columns will be flex.

   When you specify a column width, it is the width of the text, not the width of
   the column with padding and whitespace and such.

```c++
        table.setColumnFormat({
            {ionic::ColType::fixed, 4},
            {ionic::ColType::flex} });
```

3. Add text rows with `addRow()`. The number of columns in `addRow()` calls and
   `setColumnFormat()` calls must match.

```c++
        table.addRow({ "A", "B" });
        table.addRow({ "C", "D" });
```

4. Optional: Set the color and alignment of individual cells, rows, columns, or the entire table. Use `setCell()`, `setRow()`, `setColumn()`, and `setTable()`.

   `std::optional` is used here, so you can specify color and/or alignment.

```c++
        table.setCell(1, 1, { ionic::Color::red }, {});
```

5. Call `format()` to get the formatted table as a string, or `print()` to print it to the console or use the `<<` operator to print it to an ostream.

```c++
        table.print();
```

### Complete Example

More examples in the "tests.cpp" file.

```c++
    #include "ionic/ionic.h"

    ...

    ionic::TableOptions options;
    options.outerBorder = false;
    ionic::Table table(options);

    table.setColumnFormat({ 
        {ionic::ColType::fixed, 2}, 
        {ionic::ColType::flex}, 
        {ionic::ColType::flex} });
    table.addRow({ "0", "A", "The Outer World" });
    table.addRow({ "1", "Hello", "And Another" });
    table.addRow({ "2", "World", "Further Out" });

    table.print();
```

### Notes on Color

Colors are close (but not the same) between OSs and shells. There are a set of 
basic colors that are usually supported. (But there are an endless variety of
terminal shells.)

Ionic uses the approach of inserting color commands into the generated string.
This has some advantages and some disadvantages. If color isn't in your environment,
you can set the `static`:

```c++
        ionic::Table::colorEnabled = false;
```

To disable color output.

### Whitespace

Hopefully whitespace is handled "as you would expect." Nevertheless, let's
take a look.

Consider the canonical string `Hello, World\n` to
illustrate how ionic handles whitespace.

Note: The trailing newline (and other whitespace) will be discarded.

Output without breaks:

```text
Hello, World
````

Output if breaks used:

```text
Hello,
World
````

Now consider: `Hello,\nWorld\n`

The output will always be:

```text
Hello,
World
````

## Thanks

Enjoy! I hope you find it useful.
