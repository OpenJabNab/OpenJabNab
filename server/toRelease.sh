#!/bin/sh
find -name "*.pro" | xargs sed -i -e "s/debug/release/" -e "s/#DEBUG/debug/"
