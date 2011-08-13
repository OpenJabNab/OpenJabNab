#!/bin/bash

CURRENTDIR=$(dirname $0)
cd $CURRENTDIR/sources && (

	PREPROC="../preproc.pl"
	PREPROC2="../preproc_remove_extra_protos.pl"

	"$PREPROC" < main.mtl | "$PREPROC2" > "../source.mtl"
)
