#!/usr/bin/env bash

npm install

doxygen
cp -f "../README.md" "./INDEX.md"
./node_modules/.bin/moxygen ./xml --anchors
./node_modules/.bin/gitbook install
./node_modules/.bin/gitbook build
