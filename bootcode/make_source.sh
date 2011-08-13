#!/bin/bash

CURRENTDIR=$(dirname $0)
COMPILER="./compiler/mtl_linux/mtl_comp"
PREPROC="../preproc.pl"
PREPROC2="../preproc_remove_extra_protos.pl"

(
	cd $CURRENTDIR/sources
	"$PREPROC" < main.mtl | "$PREPROC2" > "../bootcode.mtl"
)

[ $? -eq 0 ] || { echo "Could not make bootcode.mtl" ; exit; }

"$COMPILER" -s "bootcode.mtl" "bootcode.bin"
