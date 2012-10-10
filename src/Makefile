BUILD_DATE := $(shell date +%F)
VERSION := $(shell cat VERSION)
DEFAULT_ARGS := VERSION=$(VERSION) BUILD_DATE=$(BUILD_DATE)

RELEASE := ../releases/Nano_MCA-$(VERSION).dfu
MCA_LIB := MCA_LIB/project/gcc
MCA_APP := MCA_APP/project/gcc

SUBDIRS := $(MCA_LIB) $(MCA_APP)

.PHONY:	all $(MCA_LIB) $(MCA_APP)

all: release

release: $(RELEASE)

upload: $(RELEASE)
	@echo
	@echo "====================================================================="
	@echo "Will try to reflash your Nano, press Ctrl+C to abort"
	@echo "Connect the Nano via USB and power it off."
	@echo "Press and hold on the "down" (\/) button and power on the Nano."
	@echo "You should see \"Firmware Upgrading...\" on its display."
	@echo "====================================================================="
	@echo "Press Enter when ready..."
	@read
	dfu-util -a 0 -D $(RELEASE)
	@echo "====================================================================="
	@echo "Done flashing, please power off the Nano!"
	@echo "Disconnect the USB cable and power back on to confirm the firmare version."
	@echo "====================================================================="
	@echo

$(RELEASE): $(MCA_LIB)/mca-lib.bin $(MCA_APP)/mca-app.bin
	../tools/dfu.py \
		-b 0x8004000:$(MCA_LIB)/mca-lib.bin \
		-b 0x800C000:$(MCA_APP)/mca-app.bin \
		$@
	@echo "The firmware is ready for upload: $@"

$(MCA_LIB)/mca-lib.bin $(MCA_APP)/mca-app.bin: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) --directory=$@ $(DEFAULT_ARGS)

clean:
	for d in $(SUBDIRS); do $(MAKE) --directory=$$d $@ $(DEFAULT_ARGS); done

distclean: clean
	for d in $(SUBDIRS); do $(MAKE) --directory=$$d $@ $(DEFAULT_ARGS); done
	rm -f $(RELEASE)

