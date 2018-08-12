#!/usr/bin/env bash

npm install https://github.com/squeek502/moxygen

doxygen
cp -f "../README.md" "./INDEX.md"
"./node_modules/.bin/moxygen" ./xml --anchors
gitbook install
gitbook build
