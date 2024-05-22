#!/bin/bash

source definitions.sh
FREQ_CMD=/opt/epics7/modules/mrfioc2/bin/linux-x86_64/FracSynthAnalyze

if [[ $# -ne 1 ]]; then
	echo "Usage: $0 <evg>"
	exit 1
fi

EVG=$1
if [[ ! $EVG =~ \/dev\/eg${VALID_EVGS}3 ]]; then
	echo "The only valid EVG names are /dev/eg${VALID_EVGS}3"
	exit 1
fi

echo ""
echo "============================================================================================"
echo "Status Dump:"
echo "============================================================================================"
$WRAP_DIR/EvgDumpStatus $EVG # TODO

echo ""
echo "============================================================================================"
echo "Fractional Divider:"
echo "============================================================================================"
FD_WORD=$($WRAP_DIR/EvgGetFracDiv $EVG)
$FREQ_CMD $FD_WORD

echo ""
echo "============================================================================================"
echo "Clock Control Register:"
echo "============================================================================================"
$WRAP_DIR/EvgDumpClockControl $EVG

echo ""
echo "============================================================================================"
echo "Sequence RAM:"
echo "============================================================================================"
echo "RAM 0"
$WRAP_DIR/EvgSeqRamStatus $EVG 0
$WRAP_DIR/EvgSeqRamDump $EVG 0

echo "Repetitions: $($WRAP_DIR/EvgSeqRamGetRepeat $EVG 0)"
echo "Start Count: $($WRAP_DIR/EvgSeqRamGetStartCnt $EVG 0)"
echo "End Count:   $($WRAP_DIR/EvgSeqRamGetEndCnt $EVG 0)"

echo ""
echo "RAM 1"
$WRAP_DIR/EvgSeqRamStatus $EVG 1
$WRAP_DIR/EvgSeqRamDump $EVG 1

echo "Repetitions: $($WRAP_DIR/EvgSeqRamGetRepeat $EVG 1)"
echo "Start Count: $($WRAP_DIR/EvgSeqRamGetStartCnt $EVG 1)"
echo "End Count:   $($WRAP_DIR/EvgSeqRamGetEndCnt $EVG 1)"

echo ""
echo "============================================================================================"
echo "Input Mapping:"
echo "============================================================================================"
$WRAP_DIR/EvgFPinDump $EVG | head -n 1
