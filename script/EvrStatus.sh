#!/bin/bash

source definitions.sh
FREQ_CMD=/opt/epics7/modules/mrfioc2/bin/linux-x86_64/FracSynthAnalyze

if [[ $# -ne 1 ]]; then
	echo "Usage: $0 <evr>"
	exit 1
fi

EVR=$1
if [[ ! $EVR =~ \/dev\/er${VALID_EVRS}3 ]]; then
	echo "The only valid EVR names are /dev/er${VALID_EVRS}3"
	exit 1
fi

echo ""
echo "============================================================================================"
echo "Status Dump:"
echo "============================================================================================"
$WRAP_DIR/EvrDumpStatus $EVR

echo ""
echo "============================================================================================"
echo "Fractional Divider:"
echo "============================================================================================"
FD_WORD=$($WRAP_DIR/EvrGetFracDiv $EVR)
$FREQ_CMD $FD_WORD

echo ""
echo "============================================================================================"
echo "Clock Control Register:"
echo "============================================================================================"
$WRAP_DIR/EvrDumpClockControl $EVR

echo ""
echo "============================================================================================"
echo "Event Mapping RAMs:"
echo "============================================================================================"
if [ "$($WRAP_DIR/EvrDumpStatus /dev/erc3 | grep MAPSEL)" ]; then
	RAM0=""
	RAM1="(selected)"
else
	RAM0="(selected)"
	RAM1=""
fi

echo "RAM 0 $RAM0"
$WRAP_DIR/EvrDumpMapRam $EVR 0

echo ""
echo "RAM 1 $RAM1"
$WRAP_DIR/EvrDumpMapRam $EVR 1

echo ""
echo "============================================================================================"
echo "Sequence RAM:"
echo "============================================================================================"
# echo "RAM 0"
$WRAP_DIR/EvrSeqRamStatus $EVR 0
$WRAP_DIR/EvrSeqRamDump $EVR 0

#echo ""
#echo "RAM 1"
#$WRAP_DIR/EvrSeqRamDump $EVR 1

echo ""
echo "============================================================================================"
echo "Pulse Generators:"
echo "============================================================================================"
$WRAP_DIR/EvrDumpPulses $EVR

echo ""
echo "============================================================================================"
echo "Output Mapping:"
echo "============================================================================================"
$WRAP_DIR/EvrDumpFPOutMap $EVR
$WRAP_DIR/EvrDumpUnivOutMap $EVR
