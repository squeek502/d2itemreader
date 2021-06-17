d2itemreader Documentation
==========================

d2itemreader's documentation uses a combination of:
- [Doxygen](http://www.doxygen.nl/) to generate XML from the d2itemreader source files
- [A fork of moxygen](https://github.com/squeek502/moxygen) to generate Markdown from the Doxygen XML file
- [gitbook-cli](https://www.npmjs.com/package/gitbook-cli) to generate the final static docs combining the generated API Markdown with the static Markdown files in the docs directory

## Building

First, you'll need to have the following installed and accessible via `PATH`:
- [node.js v12.18.2 or earlier and npm](https://nodejs.org/)
  + **Note: gitbook-cli is no longer receiving updates and is incompatible with Node v12.18.3 or later**
- [Doxygen](http://www.doxygen.nl/)

### On Windows

```
make.bat
npx gitbook serve
```

### On Linux

```
./make.sh
npx gitbook serve
```
