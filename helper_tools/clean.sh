#!/bin/sh
find -maxdepth 1 -type f | xargs -i -r -n1 file "{}" | grep ELF | cut -d ':' -f 1 | xargs -i -r -n1 rm -rf "{}"

