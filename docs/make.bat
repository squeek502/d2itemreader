@echo off

call npm install

call doxygen
copy /Y "..\README.md" ".\INDEX.md"
call npx moxygen ./xml --anchors
call npx gitbook install
call npx gitbook build
