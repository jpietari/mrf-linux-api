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

ENABLED=$($WRAP_DIR/EvrEnable $EVR 1)
if [ $ENABLED == 1 ]; then
	echo "EVR Enabled"
else
	echo "Failed to enable $EVR"
	exit 1
fi

FRACDIV=$($WRAP_DIR/EvrSetFracDiv $EVR 410029)
echo "Fractional divider word set to $FRACDIV"

$WRAP_DIR/EvrSetIntClkMode $EVR 1
echo "Clocking mode set to local"

