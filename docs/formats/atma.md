## ATMA Stash (.d2x) File Format

ATMA is the file format used by the [ATMA](http://atma.incgamers.com/) and [GoMule](http://gomule.sourceforge.net/) muling tools.

Byte Offset | Size (bytes) | Description
------------|:------------:|-------------
0x00 | 3 | File header. Must be "D2X".
0x03 | 2 | Number of items in the file as a 16 bit unsigned integer.
0x05 | 2 | File version as a 16 bit unsigned integer. Must be 96.
0x07 | 4 | ATMA checksum. See [here](https://github.com/sylecn/gomule/blob/27731580051afc7e171996997231e42a9f17cd6f/src/gomule/d2x/D2Stash.java#L176-L199) for how this is calculated.
0x0B | ... | Beginning of [D2 Item List data](d2.md#item-list-data-format) *without* the 'header' and 'number of items' fields. Instead, use the value at offset 0x03 of the ATMA data as the number of 'root' items in the list, and then read the item list as if you're starting at offset 0x04 of the item list data.

## Sources / Further Reading

* [D2Stash.java (GoMule Source Code)](https://github.com/sylecn/gomule/blob/master/src/gomule/d2x/D2Stash.java)
