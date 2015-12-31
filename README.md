# ScrumbleDataFormat
A parser for the ScrumbleDataFormat

## How to use
1. Add SdfParser.h and SdfParser.c to your file system.
2. Use sdf_parse_file("filename") to load an SDF-formatted file. It returns the root node of the abstract syntax tree (AST), which you can use to further process the loaded data.
3. IMPORTANT: When done with the AST, do not forget to free the memory using sdf_free_tree(...).

## Notes on porting from the original parser
- The main parsing function now takes a filename instead of a file handle - no need to manage opening and closing the file anymore.
- All function and data structure names have been changed to match the GTK+ naming conventions, for example:
  - AstNode is now SdfNode
  - freeTree(...) is now sdf_free_tree(...)
- Only one header and one source file.
- Different files can now safely be processed on different threads.
- The header now only contains the interface and documentation (documentation is still WIP though). The idea is that eventually the header + source file provide everything you need.

So the most important things when porting are to change the type and function names (especially SdfNode will appear a lot in your files - nothing a replace all won't handle), and to hand the parser a file name instead of a file handle. Good luck!
