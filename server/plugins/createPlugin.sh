#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: ./`basename $0` newPluginName"
  exit 65
fi

pluginLower=`echo $1 | tr [:upper:] [:lower:]`
pluginClassName="Plugin"`echo ${1:0:1} | tr [:lower:] [:upper:]`${1:1:${#1}};
pluginClassNameUpper=`echo $pluginClassName | tr [:lower:] [:upper:]`;

echo "Creating '$pluginClassName' from template in '$pluginLower' folder ..."

if [ -d $pluginLower ]; then
  echo "Folder already exists"
  exit -1
fi

mkdir $pluginLower

cp TEMPLATE/plugin_TEMPLATE.cpp $pluginLower/plugin_$pluginLower.cpp
cp TEMPLATE/plugin_TEMPLATE.h $pluginLower/plugin_$pluginLower.h
cp TEMPLATE/TEMPLATE.pro $pluginLower/$pluginLower.pro

sed -i -s "s/TEMPLATECLASSUPPER/$pluginClassNameUpper/g" $pluginLower/*
sed -i -s "s/TEMPLATECLASS/$pluginClassName/g" $pluginLower/*
sed -i -s "s/TEMPLATELOWER/$pluginLower/g" $pluginLower/*
