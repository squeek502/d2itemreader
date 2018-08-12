@echo off

call npm install https://github.com/squeek502/moxygen

call doxygen
copy /Y "../README.md" "./INDEX.md"
call "./node_modules/.bin/moxygen" ./xml --anchors
call gitbook install
call gitbook build
