CC=gcc

APIDIR=../api

all: EvFWVersion EvgEnable EvgSystemMasterEnable EvgBeaconEnable EvgRxEnable EvgGetViolation EvgSWEventEnable EvgSetMXCPrescaler EvgSetMXCTrigMap EvgSyncMxc EvgMXCDump EvgDBusDump EvgSetDBusMap EvgSetDBusEvent EvgSetACInput EvgACDump EvgSetRFInput EvgSetFracDiv EvgSetEventFrequency EvgSetSeqRamEvent EvgSeqRamDump EvgSeqRamSWTrig EvgSetUnivinMap EvgSetFPinMap EvgSetTBinMap EvgSetUnivoutMap EvgSetTBoutMap EvgTimestampEnable EvgTimestampLoad \
EvrClearFIFO \
EvrClearLog \
EvrClearPulseMap \
EvrDCEnable \
EvrDumpBPOutMap \
EvrDumpFIFO \
EvrDumpFPOutMap \
EvrDumpLog \
EvrDumpMapRam \
EvrDumpPulses \
EvrDumpTBOutMap \
EvrDumpUnivOutMap \
EvrEnable \
EvrEnableEventForwarding \
EvrGetSecondsCounter \
EvrGetSecondsLatch \
EvrGetTimestampCounter \
EvrGetTimestampLatch \
EvrGetViolation \
EvrMapRamEnable \
EvrOutputEnable \
EvrSendSWEvent \
EvrSeqRamControl \
EvrSeqRamDump \
EvrSeqRamSWTrig \
EvrSetBackDBus \
EvrSetBackEvent \
EvrSetBPOutMap \
EvrSetDBusTrig \
EvrSetEventFrequency \
EvrSetExtEvent \
EvrSetExtSensitivity \
EvrSetFIFOEvent \
EvrSetForwardEvent \
EvrSetFPOutMap \
EvrSetFracDiv \
EvrSetIntClkMode \
EvrSetLedEvent \
EvrSetLogEvent \
EvrSetLogStopEvent \
EvrSetPrescaler \
EvrSetPrescalerTrig \
EvrSetPulseMap \
EvrSetPulseMask \
EvrSetPulseParams \
EvrSetPulseProperties \
EvrSetSeqRamEvent \
EvrSetTargetDelay \
EvrSetTBOutMap \
EvrSetTimestampDBus \
EvrSetTimestampDivider \
EvrSetUnivOutMap \
EvrSWEventEnable \
EvrUnivDlyEnable \
EvrUnivDlySetDelay

EvFWVersion.o: EvFWVersion.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvFWVersion.c

EvFWVersion: EvFWVersion.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvFWVersion.o $(APIDIR)/egapi.o

EvgEnable.o: EvgEnable.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgEnable.c

EvgEnable: EvgEnable.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgEnable.o $(APIDIR)/egapi.o

EvgSystemMasterEnable.o: EvgSystemMasterEnable.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSystemMasterEnable.c

EvgSystemMasterEnable: EvgSystemMasterEnable.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSystemMasterEnable.o $(APIDIR)/egapi.o

EvgBeaconEnable.o: EvgBeaconEnable.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgBeaconEnable.c

EvgBeaconEnable: EvgBeaconEnable.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgBeaconEnable.o $(APIDIR)/egapi.o

EvgGetViolation.o: EvgGetViolation.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgGetViolation.c

EvgGetViolation: EvgGetViolation.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgGetViolation.o $(APIDIR)/egapi.o

EvgRxEnable.o: EvgRxEnable.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgRxEnable.c

EvgRxEnable: EvgRxEnable.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgRxEnable.o $(APIDIR)/egapi.o

EvgSWEventEnable.o: EvgSWEventEnable.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSWEventEnable.c

EvgSWEventEnable: EvgSWEventEnable.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSWEventEnable.o $(APIDIR)/egapi.o

EvgSendSWEvent.o: EvgSendSWEvent.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSendSWEvent.c

EvgSendSWEvent: EvgSendSWEvent.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSendSWEvent.o $(APIDIR)/egapi.o

EvgSetMXCPrescaler.o: EvgSetMXCPrescaler.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetMXCPrescaler.c

EvgSetMXCPrescaler: EvgSetMXCPrescaler.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetMXCPrescaler.o $(APIDIR)/egapi.o

EvgSetMXCTrigMap.o: EvgSetMXCTrigMap.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetMXCTrigMap.c

EvgSetMXCTrigMap: EvgSetMXCTrigMap.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetMXCTrigMap.o $(APIDIR)/egapi.o

EvgSyncMxc.o: EvgSyncMxc.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSyncMxc.c

EvgSyncMxc: EvgSyncMxc.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSyncMxc.o $(APIDIR)/egapi.o

EvgMXCDump.o: EvgMXCDump.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgMXCDump.c

EvgMXCDump: EvgMXCDump.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgMXCDump.o $(APIDIR)/egapi.o

EvgDBusDump.o: EvgDBusDump.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgDBusDump.c

EvgDBusDump: EvgDBusDump.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgDBusDump.o $(APIDIR)/egapi.o

EvgSetDBusMap.o: EvgSetDBusMap.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetDBusMap.c

EvgSetDBusMap: EvgSetDBusMap.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetDBusMap.o $(APIDIR)/egapi.o

EvgSetDBusEvent.o: EvgSetDBusEvent.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetDBusEvent.c

EvgSetDBusEvent: EvgSetDBusEvent.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetDBusEvent.o $(APIDIR)/egapi.o

EvgSetACInput.o: EvgSetACInput.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetACInput.c

EvgSetACInput: EvgSetACInput.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetACInput.o $(APIDIR)/egapi.o

EvgACDump.o: EvgACDump.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgACDump.c

EvgACDump: EvgACDump.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgACDump.o $(APIDIR)/egapi.o

EvgSetRFInput.o: EvgSetRFInput.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetRFInput.c

EvgSetRFInput: EvgSetRFInput.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetRFInput.o $(APIDIR)/egapi.o

EvgSetFracDiv.o: EvgSetFracDiv.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetFracDiv.c

EvgSetFracDiv: EvgSetFracDiv.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetFracDiv.o $(APIDIR)/egapi.o

EvgSetEventFrequency.o: EvgSetEventFrequency.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetEventFrequency.c

EvgSetEventFrequency: EvgSetEventFrequency.o $(APIDIR)/egapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvgSetEventFrequency.o $(APIDIR)/egapi.o $(APIDIR)/fracdiv.o

EvgSetSeqRamEvent.o: EvgSetSeqRamEvent.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetSeqRamEvent.c

EvgSetSeqRamEvent: EvgSetSeqRamEvent.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetSeqRamEvent.o $(APIDIR)/egapi.o

EvgSeqRamDump.o: EvgSeqRamDump.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSeqRamDump.c

EvgSeqRamDump: EvgSeqRamDump.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSeqRamDump.o $(APIDIR)/egapi.o

EvgSeqRamSWTrig.o: EvgSeqRamSWTrig.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSeqRamSWTrig.c

EvgSeqRamSWTrig: EvgSeqRamSWTrig.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSeqRamSWTrig.o $(APIDIR)/egapi.o

EvgSetUnivinMap.o: EvgSetUnivinMap.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetUnivinMap.c

EvgSetUnivinMap: EvgSetUnivinMap.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetUnivinMap.o $(APIDIR)/egapi.o

EvgSetUnivoutMap.o: EvgSetUnivoutMap.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetUnivoutMap.c

EvgSetUnivoutMap: EvgSetUnivoutMap.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetUnivoutMap.o $(APIDIR)/egapi.o

EvgSetTBinMap.o: EvgSetTBinMap.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetTBinMap.c

EvgSetTBinMap: EvgSetTBinMap.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetTBinMap.o $(APIDIR)/egapi.o

EvgSetTBoutMap.o: EvgSetTBoutMap.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetTBoutMap.c

EvgSetTBoutMap: EvgSetTBoutMap.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetTBoutMap.o $(APIDIR)/egapi.o

EvgSetFPinMap.o: EvgSetFPinMap.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgSetFPinMap.c

EvgSetFPinMap: EvgSetFPinMap.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgSetFPinMap.o $(APIDIR)/egapi.o

EvgTimestampEnable.o: EvgTimestampEnable.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgTimestampEnable.c

EvgTimestampEnable: EvgTimestampEnable.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgTimestampEnable.o $(APIDIR)/egapi.o

EvgTimestampLoad.o: EvgTimestampLoad.c $(APIDIR)/egapi.h
	$(CC) $(CFLAGS) -c EvgTimestampLoad.c

EvgTimestampLoad: EvgTimestampLoad.o $(APIDIR)/egapi.o
	$(CC) $(LDFLAGS) -o $@ EvgTimestampLoad.o $(APIDIR)/egapi.o

EvrEnable.o: EvrEnable.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrEnable.c

EvrEnable: EvrEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrDCEnable.o: EvrDCEnable.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrDCEnable.c

EvrDCEnable: EvrDCEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrDCEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrClearFIFO.o: EvrClearFIFO.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrClearFIFO.c

EvrClearFIFO: EvrClearFIFO.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrClearFIFO.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrClearLog.o: EvrClearLog.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrClearLog.c

EvrClearLog: EvrClearLog.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrClearLog.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrClearPulseMap.o: EvrClearPulseMap.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrClearPulseMap.c

EvrClearPulseMap: EvrClearPulseMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrClearPulseMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrDumpBPOutMap.o: EvrDumpBPOutMap.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrDumpBPOutMap.c

EvrDumpBPOutMap: EvrDumpBPOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrDumpBPOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrDumpFIFO.o: EvrDumpFIFO.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrDumpFIFO.c

EvrDumpFIFO: EvrDumpFIFO.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrDumpFIFO.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrDumpFPOutMap.o: EvrDumpFPOutMap.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrDumpFPOutMap.c

EvrDumpFPOutMap: EvrDumpFPOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrDumpFPOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrDumpLog.o: EvrDumpLog.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrDumpLog.c

EvrDumpLog: EvrDumpLog.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrDumpLog.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrDumpMapRam.o: EvrDumpMapRam.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrDumpMapRam.c

EvrDumpMapRam: EvrDumpMapRam.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrDumpMapRam.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrDumpPulses.o: EvrDumpPulses.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrDumpPulses.c

EvrDumpPulses: EvrDumpPulses.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrDumpPulses.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrDumpTBOutMap.o: EvrDumpTBOutMap.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrDumpTBOutMap.c

EvrDumpTBOutMap: EvrDumpTBOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrDumpTBOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrDumpUnivOutMap.o: EvrDumpUnivOutMap.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrDumpUnivOutMap.c

EvrDumpUnivOutMap: EvrDumpUnivOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrDumpUnivOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrEnableEventForwarding.o: EvrEnableEventForwarding.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrEnableEventForwarding.c

EvrEnableEventForwarding: EvrEnableEventForwarding.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrEnableEventForwarding.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrGetSecondsCounter.o: EvrGetSecondsCounter.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrGetSecondsCounter.c

EvrGetSecondsCounter: EvrGetSecondsCounter.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrGetSecondsCounter.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrGetSecondsLatch.o: EvrGetSecondsLatch.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrGetSecondsLatch.c

EvrGetSecondsLatch: EvrGetSecondsLatch.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrGetSecondsLatch.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrGetTimestampCounter.o: EvrGetTimestampCounter.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrGetTimestampCounter.c

EvrGetTimestampCounter: EvrGetTimestampCounter.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrGetTimestampCounter.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrGetTimestampLatch.o: EvrGetTimestampLatch.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrGetTimestampLatch.c

EvrGetTimestampLatch: EvrGetTimestampLatch.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrGetTimestampLatch.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrGetViolation.o: EvrGetViolation.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrGetViolation.c

EvrGetViolation: EvrGetViolation.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrGetViolation.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrMapRamEnable.o: EvrMapRamEnable.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrMapRamEnable.c

EvrMapRamEnable: EvrMapRamEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrMapRamEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrOutputEnable.o: EvrOutputEnable.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrOutputEnable.c

EvrOutputEnable: EvrOutputEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrOutputEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSendSWEvent.o: EvrSendSWEvent.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSendSWEvent.c

EvrSendSWEvent: EvrSendSWEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSendSWEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSeqRamControl.o: EvrSeqRamControl.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSeqRamControl.c

EvrSeqRamControl: EvrSeqRamControl.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSeqRamControl.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSeqRamDump.o: EvrSeqRamDump.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSeqRamDump.c

EvrSeqRamDump: EvrSeqRamDump.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSeqRamDump.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSeqRamSWTrig.o: EvrSeqRamSWTrig.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSeqRamSWTrig.c

EvrSeqRamSWTrig: EvrSeqRamSWTrig.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSeqRamSWTrig.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetBackDBus.o: EvrSetBackDBus.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetBackDBus.c

EvrSetBackDBus: EvrSetBackDBus.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetBackDBus.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetBackEvent.o: EvrSetBackEvent.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetBackEvent.c

EvrSetBackEvent: EvrSetBackEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetBackEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetBPOutMap.o: EvrSetBPOutMap.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetBPOutMap.c

EvrSetBPOutMap: EvrSetBPOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetBPOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetDBusTrig.o: EvrSetDBusTrig.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetDBusTrig.c

EvrSetDBusTrig: EvrSetDBusTrig.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetDBusTrig.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetEventFrequency.o: EvrSetEventFrequency.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetEventFrequency.c

EvrSetEventFrequency: EvrSetEventFrequency.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetEventFrequency.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetExtEvent.o: EvrSetExtEvent.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetExtEvent.c

EvrSetExtEvent: EvrSetExtEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetExtEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetExtSensitivity.o: EvrSetExtSensitivity.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetExtSensitivity.c

EvrSetExtSensitivity: EvrSetExtSensitivity.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetExtSensitivity.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetFIFOEvent.o: EvrSetFIFOEvent.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetFIFOEvent.c

EvrSetFIFOEvent: EvrSetFIFOEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetFIFOEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetForwardEvent.o: EvrSetForwardEvent.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetForwardEvent.c

EvrSetForwardEvent: EvrSetForwardEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetForwardEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetFPOutMap.o: EvrSetFPOutMap.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetFPOutMap.c

EvrSetFPOutMap: EvrSetFPOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetFPOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetFracDiv.o: EvrSetFracDiv.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetFracDiv.c

EvrSetFracDiv: EvrSetFracDiv.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetFracDiv.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetIntClkMode.o: EvrSetIntClkMode.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetIntClkMode.c

EvrSetIntClkMode: EvrSetIntClkMode.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetIntClkMode.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetLedEvent.o: EvrSetLedEvent.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetLedEvent.c

EvrSetLedEvent: EvrSetLedEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetLedEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetLogEvent.o: EvrSetLogEvent.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetLogEvent.c

EvrSetLogEvent: EvrSetLogEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetLogEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetLogStopEvent.o: EvrSetLogStopEvent.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetLogStopEvent.c

EvrSetLogStopEvent: EvrSetLogStopEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetLogStopEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetPrescaler.o: EvrSetPrescaler.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetPrescaler.c

EvrSetPrescaler: EvrSetPrescaler.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetPrescaler.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetPrescalerTrig.o: EvrSetPrescalerTrig.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetPrescalerTrig.c

EvrSetPrescalerTrig: EvrSetPrescalerTrig.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetPrescalerTrig.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetPulseMap.o: EvrSetPulseMap.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetPulseMap.c

EvrSetPulseMap: EvrSetPulseMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetPulseMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetPulseMask.o: EvrSetPulseMask.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetPulseMask.c

EvrSetPulseMask: EvrSetPulseMask.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetPulseMask.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetPulseParams.o: EvrSetPulseParams.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetPulseParams.c

EvrSetPulseParams: EvrSetPulseParams.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetPulseParams.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetPulseProperties.o: EvrSetPulseProperties.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetPulseProperties.c

EvrSetPulseProperties: EvrSetPulseProperties.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetPulseProperties.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetSeqRamEvent.o: EvrSetSeqRamEvent.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetSeqRamEvent.c

EvrSetSeqRamEvent: EvrSetSeqRamEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetSeqRamEvent.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetTargetDelay.o: EvrSetTargetDelay.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetTargetDelay.c

EvrSetTargetDelay: EvrSetTargetDelay.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetTargetDelay.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetTBOutMap.o: EvrSetTBOutMap.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetTBOutMap.c

EvrSetTBOutMap: EvrSetTBOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetTBOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetTimestampDBus.o: EvrSetTimestampDBus.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetTimestampDBus.c

EvrSetTimestampDBus: EvrSetTimestampDBus.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetTimestampDBus.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetTimestampDivider.o: EvrSetTimestampDivider.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetTimestampDivider.c

EvrSetTimestampDivider: EvrSetTimestampDivider.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetTimestampDivider.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSetUnivOutMap.o: EvrSetUnivOutMap.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSetUnivOutMap.c

EvrSetUnivOutMap: EvrSetUnivOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSetUnivOutMap.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrSWEventEnable.o: EvrSWEventEnable.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrSWEventEnable.c

EvrSWEventEnable: EvrSWEventEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrSWEventEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrUnivDlyEnable.o: EvrUnivDlyEnable.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrUnivDlyEnable.c

EvrUnivDlyEnable: EvrUnivDlyEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrUnivDlyEnable.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

EvrUnivDlySetDelay.o: EvrUnivDlySetDelay.c $(APIDIR)/erapi.h $(APIDIR)/fracdiv.h
	$(CC) $(CFLAGS) -c EvrUnivDlySetDelay.c

EvrUnivDlySetDelay: EvrUnivDlySetDelay.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o
	$(CC) $(LDFLAGS) -o $@ EvrUnivDlySetDelay.o $(APIDIR)/erapi.o $(APIDIR)/fracdiv.o

clean:
	rm -rf *.o *~ EvFWVersion EvgEnable EvgSystemMasterEnable EvgBeaconEnable EvgRxEnable EvgGetViolation EvgSWEventEnable EvgSendSWEvent EvgSetMXCPrescaler EvgSetMXCTrigMap EvgSyncMxc EvgMXCDump EvgDBusDump EvgSetDBusMap EvgSetDBusEvent EvgSetACInput EvgACDump EvgSetRFInput EvgSetFracDiv EvgSetEventFrequency EvgSetSeqRamEvent EvgSeqRamDump EvgSeqRamSWTrig EvgSetUnivinMap EvgSetFPinMap EvgSetTBinMap EvgSetUnivoutMap EvgSetTBoutMap EvgTimestampEnable EvgTimestampLoad \
EvrClearFIFO \
EvrClearLog \
EvrClearPulseMap \
EvrDCEnable \
EvrDumpBPOutMap \
EvrDumpFIFO \
EvrDumpFPOutMap \
EvrDumpLog \
EvrDumpMapRam \
EvrDumpPulses \
EvrDumpTBOutMap \
EvrDumpUnivOutMap \
EvrEnable \
EvrEnableEventForwarding \
EvrGetSecondsCounter \
EvrGetSecondsLatch \
EvrGetTimestampCounter \
EvrGetTimestampLatch \
EvrGetViolation \
EvrMapRamEnable \
EvrOutputEnable \
EvrSendSWEvent \
EvrSeqRamControl \
EvrSeqRamDump \
EvrSeqRamSWTrig \
EvrSetBackDBus \
EvrSetBackEvent \
EvrSetBPOutMap \
EvrSetDBusTrig \
EvrSetEventFrequency \
EvrSetExtEvent \
EvrSetExtSensitivity \
EvrSetFIFOEvent \
EvrSetForwardEvent \
EvrSetFPOutMap \
EvrSetFracDiv \
EvrSetIntClkMode \
EvrSetLedEvent \
EvrSetLogEvent \
EvrSetLogStopEvent \
EvrSetPrescaler \
EvrSetPrescalerTrig \
EvrSetPulseMap \
EvrSetPulseMask \
EvrSetPulseParams \
EvrSetPulseProperties \
EvrSetSeqRamEvent \
EvrSetTargetDelay \
EvrSetTBOutMap \
EvrSetTimestampDBus \
EvrSetTimestampDivider \
EvrSetUnivOutMap \
EvrSWEventEnable \
EvrUnivDlyEnable \
EvrUnivDlySetDelay \
