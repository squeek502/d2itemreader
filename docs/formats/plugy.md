## PlugY Shared Stash (.sss) File Format

Byte Offset | Size (bytes) | Description
------------|:------------:|-------------
0x00 | 4 | File header. Must be "SSS\0" (0x00535353 when read as a 32 bit unsigned integer).
0x04 | 2 | File version. Will be "01" (0x3130) when file version is 1, or "02" (0x3230) when file version is 2.

#### File version 1

When the file version is "01", there will be no shared gold data in the file.

Byte Offset | Size (bytes) | Description
------------|:------------:|-------------
0x06 | 4 | Number of pages in the stash data as a 32 bit unsigned integer
0x0A | ... | Beginning of contiguous [Stash data](#stash-data-format)

#### File version 2

When the file version is "02", there will be shared gold data in the file.

Byte Offset | Size (bytes) | Description
------------|:------------:|-------------
0x06 | 4 | Shared gold as a 32 bit unsigned integer
0x0A | 4 | Number of pages in the stash data as a 32 bit unsigned integer
0x0E | ... | Beginning of contiguous [Stash data](#stash-data-format)

## PlugY Personal Stash (.d2x) File Format

Byte Offset | Size (bytes) | Description
------------|:------------:|-------------
0x00 | 4 | File header. Must be "CSTM" (0x4D545343 when read as a 32 bit unsigned integer).
0x04 | 2 | File version. Will be "01" (0x3130) when file version is 1. No other versions are in use currently.
0x06 | 4 | Unused bytes
0x0A | 4 | Number of pages in the stash data as a 32 bit unsigned integer
0x0E | ... | Beginning of contiguous [Stash data](#stash-data-format)

## Stash Data Format

For each stash page:

Byte Offset | Size (bytes) | Description
------------|:------------:|-------------
0x00 | 2 | Stash header. Must be "ST" (0x5453 when read as a 16 bit unsigned integer).
0x02 | 0 or 4 | Flags data (added in PlugY 11.02, doesn't exist in save data from earlier versions). The actual flags can be seen [here](https://github.com/ChaosMarc/PlugY/blob/0116cb44b459ba02832cf8f07092ce4f48aeecdf/PlugY/playerCustomData.h#L19-L27). <br/><br/> It's possible to determine if the flags data exists by checking if the 2 bytes following the next null character == the D2 item header ("JM"); see [the PlugY source code for an example](https://github.com/ChaosMarc/PlugY/blob/0116cb44b459ba02832cf8f07092ce4f48aeecdf/PlugY/InfinityStash.cpp#L258-L264).
0x02 or 0x06 | At least 1 | Stash page name as a null-terminated string. As of PlugY 14.00, this can be a maximum of 20 characters + the null terminator (21 total bytes). In previous versions, the maximum was 15 characters + the null terminator (16 total bytes).
Varies | ... | Beginning of [D2 Item List data](d2.html#item-list-data-format)

## Sources / Further Reading

* [PlugY save files structures (The Phrozen Keep)](https://d2mods.info/forum/viewtopic.php?f=133&t=31359)
* [loadStash (PlugY 11.02 Source Code)](https://github.com/ChaosMarc/PlugY/blob/0116cb44b459ba02832cf8f07092ce4f48aeecdf/PlugY/InfinityStash.cpp#L243-L282)
