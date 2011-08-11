#!/bin/bash

CURRENTDIR=$(dirname $0)
cd $CURRENTDIR

PREPROC="../preproc.pl"
PREPROC2="../preproc_remove_extra_protos.pl"

"$PREPROC" < main.mtl | "$PREPROC2" > "../nominal.mtl"
