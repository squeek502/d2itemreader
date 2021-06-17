#!/usr/bin/env bash

npm install

doxygen
cp -f "../README.md" "./INDEX.md"
npx moxygen ./xml --anchors
npx gitbook install
npx gitbook build
