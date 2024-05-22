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

CODE=1
PULSE=31
OUTPUT=2
TRIG=61

# Setup sequence RAM 0      dev	 ram  en  single recycle reset trigsel mask
$WRAP_DIR/EvgSeqRamControl  $EVG 0    0   0      0       1     0       0
#							dev  ram  i   time   code
$WRAP_DIR/EvgSetSeqRamEvent $EVG 0    0   100    $CODE
$WRAP_DIR/EvgSetSeqRamEvent $EVG 0    1   350    $CODE
$WRAP_DIR/EvgSetSeqRamEvent $EVG 0    2   600    127

$WRAP_DIR/EvgSeqRamControl  $EVG 0    1   0      0       0     17      0


# Setup sequence RAM 1      dev	 ram  en  single recycle reset trigsel mask
$WRAP_DIR/EvgSeqRamControl  $EVG 1    0   0      1       1     0       0
#							dev  ram  i   time   code
$WRAP_DIR/EvgSetSeqRamEvent $EVG 1    0   100    $CODE
$WRAP_DIR/EvgSetSeqRamEvent $EVG 1    1   350    $CODE
$WRAP_DIR/EvgSetSeqRamEvent $EVG 1    1   700    $CODE
$WRAP_DIR/EvgSetSeqRamEvent $EVG 1    2   1000    127

$WRAP_DIR/EvgSeqRamControl  $EVG 1    1   1      0       0     18      0

# Enable interrupts (0x00003300)
$WRAP_DIR/EvgIrqEnable $EVG 13056


