#!/bin/sh

# Ce script permet de corriger automatiquement, dans un code source
# mtl, les variables déclarées en "var" qui devraient être déclarées
# en const.
# 
# Cependant il vaut mieux regarder "à la main" d'abord, ou tout du
# moins regarder les modifications qui sont faites, pour identifier
# les cas où un utilise des constantes alors qu'on devrait utiliser un
# type énuméré.

usage() {
		echo "usage: $(basename $0) <logs_de_la_compilation> " \
		     "<fichier_source_compile> <fichier_source_corrige>"
}

if [ "$#" -ne 3 ] ; then
		usage
		return
fi

TMPFILE="$(mktemp)"

cp "$2" "${TMPFILE}"

for i in $(grep ' is only set once, at declaration. It should be a const.' "$1" | cut -d' ' -f1 | tr -s \\n ' ') ; do
		echo -n "replacing $i..."
		sed  -i s/"var $i\([ =;]\)"/"const $i\1"/g "$TMPFILE"
		echo "done !"
done

mv "${TMPFILE}" "$3"

echo "all done !"
