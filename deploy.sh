#!/bin/bash
git checkout production && \
git merge master && \
make engine.js && \
git add -f engine.js engine.wasm && \
git commit -m "Deploy: $(date)" && \
git push origin production && \
git checkout master
