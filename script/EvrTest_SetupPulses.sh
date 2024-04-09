#!/bin/bash

source definitions.sh

if [[ $# -ne 1 ]]; then
	echo "Usage: $0 <evr>"
	exit 1
fi

EVR=$1
if [[ ! $EVR =~ \/dev\/er${VALID_EVRS}3 ]]; then
	echo "The only valid EVR names are /dev/er${VALID_EVRS}3"
	exit 1
fi

RAM=0
CODE=1
PULSE=31
OUTPUT=2
TRIG=61

# Setup event mapping RAM
$WRAP_DIR/EvrSetPulseMap $EVR $RAM $CODE $PULSE -1 -1
$WRAP_DIR/EvrMapRamEnable $EVR $RAM 1

# Setup Pulsers and Output channels
$WRAP_DIR/EvrSetPulseParams $EVR $PULSE 1 100 100
$WRAP_DIR/EvrSetPulseProperties $EVR $PULSE 0 0 0 1 1
$WRAP_DIR/EvrSetUnivOutMap $EVR $OUTPUT $PULSE

# Setup sequence RAM
$WRAP_DIR/EvrSeqRamControl $EVR $RAM 0 0 0 1 0
$WRAP_DIR/EvrSetSeqRamEvent $EVR $RAM 0 100 $CODE
$WRAP_DIR/EvrSetSeqRamEvent $EVR $RAM 1 350 $CODE
$WRAP_DIR/EvrSetSeqRamEvent $EVR $RAM 2 600 127
$WRAP_DIR/EvrSeqRamControl $EVR $RAM 1 0 0 0 $TRIG

