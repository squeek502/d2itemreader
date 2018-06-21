d2itemreader
============

[![Build Status](https://travis-ci.org/squeek502/d2itemreader.svg?branch=master)](https://travis-ci.org/squeek502/d2itemreader)

**work in progress, everything is subject to change**

d2itemreader is a C library for parsing Diablo II character/stash files (`.d2s`, `.d2x`, and `.sss`) and retrieving data about the items contained inside them. It also tries to avoid any assumptions about the game version or game data, so that it can work with modded files (provided the library is initialized with the relevant modded .txt files on startup).

## Acknowledgements

- [nokka/d2s](https://github.com/nokka/d2s) - much of the d2s parsing of d2itemreader is ported from `nokka/d2s`
