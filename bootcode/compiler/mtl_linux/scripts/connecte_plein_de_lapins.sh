#!/bin/bash

# Ce script connecte N nouveaux lapins au serveur, en simulant une
# config sans mot de passe

# si cette variable est renseignée, on ira chercher les adresses MAC à
# utiliser dedans. Sinon, on les génèrera.
MAC_ADDRS_FILE="/tmp/MACS"

# le port à partir duquel on va ouvrir des connections entrantes (on
# ouvrira des ports de PORT_BASE à PORT_BASE + nb_lapins)
PORT_BASE=6000

if [ -z "$MAC_ADDRS_FILE" ] ; then
## génération des adresses MAC

# le format des adresses MAC avec lesquelles on va essayer de
# s'enregistrer
		MAC_FORMAT="0000030%05d"
# le nombre de lapins à generer (si >= 10000, penser à changer le
# format pour garder des MAC de 12 caracteres
		NB_RABBITS_GEN=12

		MAC_ADDRS=""
		NB_RABBITS=0
		for ((i=0; i<"$NB_RABBITS_GEN"; ++i)) ; do
				MAC_ADDRS=$(printf "$MAC_ADDRS $MAC_FORMAT" $i)
				NB=$(($NB+1))
		done
else
## lecture des adresses MAC dans le fichier
		MAC_ADDRS=$(cat "$MAC_ADDRS_FILE")
fi

# on se place dans le rep de base
cd ..
BASEDIR="$(pwd)"

echo "This script will create a directory $BASEDIR/lapins"

echo -n "Removing old logs, sources, and confs... "
rm -rf lapins/*
echo "done"

echo -n > PIDS

CONF_ORIG=$(mktemp)
cp conf.bin "$CONF_ORIG"

i=0
for MAC in $MAC_ADDRS ; do
		mkdir -p $BASEDIR/lapins/$i
		cd $BASEDIR/lapins/$i
		echo "MAC: " $MAC > $BASEDIR/lapins/$i/MAC
		# adresse mac unique
		#MAC=$(printf "$MAC_FORMAT" "$i");
		cp $BASEDIR/conf.bin.sans_password conf.bin
		cp $BASEDIR/config.txt config.txt
		cp $BASEDIR/nominal.mtl nominal.mtl
		# DNSLOCAL a changer
		sed -i s/1597/"$((1597 + $i))"/g nominal.mtl
		# go !
		echo -n "($i) Connecting with mac $MAC... ";
		# le echo "  " est pour eviter que le simulateur ne se bloque a cause des getchar a la fin de la compile et au debut de la simu
		echo "  " | $BASEDIR/mtl_simu --mac $MAC > log.$i 2>&1  &
		echo $! >> $BASEDIR/PIDS
		echo "done"
		sleep 0.2
		i=$((i+1))
done;

echo "Press a key to kill all (virtual) rabbits."
read

## killer tout le monde
cd $BASEDIR
for pid in $(cat PIDS) ; do
		echo -n "Killing $pid... "
		kill -9 "$pid"
		echo "done"
done

# cleaner un peu
rm PIDS
mv $CONF_ORIG conf.bin

# afficher la commande pour tous les virer de ejabberd
echo "To remove all jabber accounts created by this script, log on nabdev and type"
echo "for ((i=0; i<$NB_RABBITS; ++i)) ; do TMP=\$(printf \"$MAC_FORMAT\" \"\$i\") ; /usr/local/ejabberd/ejabberd-current/sbin/ejabberdctl unregister \"\$TMP\" xmpp-dev.nabaztag.com ; done"
