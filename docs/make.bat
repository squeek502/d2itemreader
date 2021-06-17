@echo off

call npm install

call doxygen
copy /Y "..\README.md" ".\INDEX.md"
call "./node_modules/.bin/moxygen" ./xml --anchors
call "./node_modules/.bin/gitbook" install
call "./node_modules/.bin/gitbook" build
