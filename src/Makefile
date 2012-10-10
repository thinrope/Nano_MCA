VERSION := $(shell cat VERSION)
RELEASE := ../releases/Nano_MCA-$(VERSION).dfu
MCA_LIB := MCA_LIB/project/gcc
MCA_APP := MCA_APP/project/gcc

SUBDIRS := $(MCA_LIB) $(MCA_APP)

.PHONY:	all $(MCA_LIB) $(MCA_APP)

all:	release

release:	$(RELEASE)	

$(RELEASE):	$(MCA_LIB)/mca-lib.bin $(MCA_APP)/mca-app.bin
	../tools/dfu.py \
		-b 0x8004000:$(MCA_LIB)/mca-lib.bin \
		-b 0x800C000:$(MCA_APP)/mca-app.bin \
		$@
	@echo "The firmware is ready for upload: $@"

$(MCA_LIB)/mca-lib.bin $(MCA_APP)/mca-app.bin:	$(SUBDIRS)

$(SUBDIRS):
	$(MAKE) --directory=$@ $(TARGET)

clean:
	for d in $(SUBDIRS); do $(MAKE) --directory=$$d $@; done

distclean:	clean
	for d in $(SUBDIRS); do $(MAKE) --directory=$$d $@; done
	rm -f $(RELEASE)

