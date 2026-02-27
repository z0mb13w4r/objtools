CROSS = I386

ifeq ($(DEBUG),y)
else
	DEBUG = n
endif

ifeq ($(PROFILE),y)
else
	PROFILE = n
endif

all:
	$(MAKE) -f objdwarf-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) all
	$(MAKE) -f objhash-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) all
	$(MAKE) -f readelf-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) all
	$(MAKE) -f objdump-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) all
	$(MAKE) -f objcopy-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) all
	$(MAKE) -f convert-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) all
	$(MAKE) -f readpe-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) all

clean:
	$(MAKE) -f objdwarf-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) clean
	$(MAKE) -f objhash-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) clean
	$(MAKE) -f readelf-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) clean
	$(MAKE) -f objdump-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) clean
	$(MAKE) -f objcopy-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) clean
	$(MAKE) -f convert-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) clean
	$(MAKE) -f readpe-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) clean

cleanall:
	$(MAKE) -f objtoollib.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) cleanall
	$(MAKE) -f objdwarf-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) cleanall
	$(MAKE) -f objhash-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) cleanall
	$(MAKE) -f readelf-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) cleanall
	$(MAKE) -f objdump-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) cleanall
	$(MAKE) -f objcopy-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) cleanall
	$(MAKE) -f convert-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) cleanall
	$(MAKE) -f readpe-ng.mk DEBUG=$(DEBUG) CROSS=$(CROSS) PROFILE=$(PROFILE) cleanall

