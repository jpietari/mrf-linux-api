CC=gcc

APIDIR=../api

APIHEADERS := $(APIDIR)/egapi.h $(APIDIR)/erapi.h $(APIDIR)/fctapi.h \
              $(APIDIR)/fracdiv.h $(APIDIR)/sfpdiag.h

APIOBJECTS := $(APIDIR)/egapi.o $(APIDIR)/erapi.o $(APIDIR)/fctapi.o \
              $(APIDIR)/fracdiv.o $(APIDIR)/sfpdiag.o

WRAPPERS := EvFWVersion EvgEnable EvgSystemMasterEnable EvgBeaconEnable EvgRxEnable EvgGetViolation EvgSWEventEnable EvgSetMXCPrescaler EvgSetMXCTrigMap EvgSyncMxc EvgMXCDump EvgDBusDump EvgSetDBusMap EvgSetDBusEvent EvgSetACInput EvgACDump EvgSetRFInput EvgSetFracDiv EvgSetEventFrequency EvgSetSeqRamEvent EvgSeqRamDump EvgSeqRamSWTrig EvgSetUnivinMap EvgSetFPinMap EvgSetTBinMap EvgSetUnivoutMap EvgSetTBoutMap EvgTimestampEnable EvgTimestampLoad \
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
EvrShowInputState \
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
FctDumpStatus \
FctGetViolation

all: $(WRAPPERS)

% : %.c

% : %.o $(APIOBJECTS)
	$(CC) $(LDFLAGS) -o $@ $< $(APIOBJECTS)

%.o : %.c $(APIHEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~ $(WRAPPERS)
