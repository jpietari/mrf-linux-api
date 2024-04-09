#!/bin/bash

source definitions.sh

if [[ $# -ne 1 ]]; then
	echo "Usage: $0 <evg>"
	exit 1
fi

EVG=$1
if [[ ! $EVG =~ \/dev\/eg${VALID_EVGS}3 ]]; then
	echo "The only valid EVG names are /dev/eg${VALID_EVGS}3"
	exit 1
fi

$WRAP_DIR/EvgEnable $EVG 0 > dev/null

# ARG[2]: 0=Internal 4=Upstream
$WRAP_DIR/EvgSetRFInput $EVG 0 12
echo "Clocking mode set to Fan-Out"

FRACDIV=$($WRAP_DIR/EvgSetFracDiv $EVG 11452781)
echo "Fractional divider word set to $FRACDIV"

ENABLED=$($WRAP_DIR/EvgEnable $EVG 1)
if [ $ENABLED == 1 ]; then
	echo "EVG Enabled"
else
	echo "Failed to enable $EVG"
	exit 1
fi
