#!/bin/sh
find -name "*.pro" | xargs sed -i -e "s/debug/#DEBUG/" -e "s/release/debug/"
