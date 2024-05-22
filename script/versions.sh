#!/bin/bash

echo ""

EVGS=$(ls /dev/eg*3)
echo "Event Generators"
for f in $EVGS
do
	VER=$(../wrapper/EvgFWVersion $f)
	echo "	$f : $VER"
done

EVRS=$(ls /dev/er*3)
echo "Event Receivers"
for f in $EVRS
do
	VER=$(../wrapper/EvrFWVersion $f)
	echo "	$f : $VER"
done

