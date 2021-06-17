d2itemreader Documentation
==========================

d2itemreader's documentation uses a combination of:
- [Doxygen](http://www.doxygen.nl/) to generate XML from the d2itemreader source files
- [A fork of moxygen](https://github.com/squeek502/moxygen) to generate Markdown from the Doxygen XML file
- [gitbook-cli](https://www.npmjs.com/package/gitbook-cli) to generate the final static docs combining the generated API Markdown with the static Markdown files in the docs directory

**Note: gitbook-cli is no longer receiving updates and does not seem to work with versions of Node.js newer than 10.x**

## Building

First, you'll need to have the following installed and accessible via `PATH`:
- [node.js 10.x and npm](https://nodejs.org/)
- [Doxygen](http://www.doxygen.nl/)

### On Windows

```
make.bat
node_modules/.bin/gitbook serve
```

### On Linux

```
./make.sh
./node_modules/.bin/gitbook serve
```
