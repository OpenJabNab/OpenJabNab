#!/bin/bash

CURRENTDIR=$(dirname $0)
cd $CURRENTDIR

COMPILER="$HOME/work/compilateur/trunk/mtl_linux/mtl_comp"

./make_nominal.sh

[ $? -eq 0 ] || { echo "Could not make nominal.mtl" ;  }

"$COMPILER" -s "../nominal.mtl" "/var/www/bytecode"
