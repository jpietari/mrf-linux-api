CC=gcc

APIDIR=../api

APIHEADERS := $(APIDIR)/egapi.h $(APIDIR)/erapi.h $(APIDIR)/fctapi.h \
              $(APIDIR)/fracdiv.h $(APIDIR)/sfpdiag.h

APIOBJECTS := $(APIDIR)/egapi.o $(APIDIR)/erapi.o $(APIDIR)/fctapi.o \
              $(APIDIR)/fracdiv.o $(APIDIR)/sfpdiag.o

WRAPPERS := EvFWVersion EvgEnable EvgSystemMasterEnable EvgBeaconEnable EvgRxEnable EvgGetViolation EvgSendSWEvent EvgSWEventEnable EvgSetMXCPrescaler EvgSetMXCTrigMap EvgSyncMxc EvgMXCDump EvgDBusDump EvgSetDBusMap EvgSetDBusEvent EvgSetACInput EvgSetACMap EvgACDump EvgSetRFInput EvgSetFracDiv EvgSetEventFrequency EvgSetSeqRamEvent EvgSeqRamStatus EvgSeqRamDump EvgSeqRamSWTrig EvgSetUnivinMap EvgSetFPinMap EvgFPinDump EvgSetBPinMap EvgSetTBinMap EvgSetUnivoutMap EvgSetBPoutMap EvgSetTBoutMap EvgTimestampEnable EvgTimestampLoad \
EvgSeqRamControl \
EvgTBinDump \
EvgUnivinDump \
EvgSetTriggerEvent \
EvrClearFIFO \
EvrClearLog \
EvrClearPulseMap \
EvrDCEnable \
EvrDumpBPOutMap \
EvrDumpFIFO \
EvrDumpFPOutMap \
EvrDumpDC \
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
EvrSetPrescalerPolarity \
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

all: $(WRAPPERS) doc

wrappers: $(WRAPPERS)

% : %.c

% : %.o $(APIOBJECTS)
	$(CC) $(LDFLAGS) -o $@ $< $(APIOBJECTS)

%.o : %.c $(APIHEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~ $(WRAPPERS) latex html

doc: 
	doxygen
	$(MAKE) -C latex -f Makefile
