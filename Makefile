#make files.
PWD := $(shell pwd)

CROSS_COMPILE = 
#CROSS_COMPILE = arm-linux-gnueabihf-

ifeq (${CROSS_COMPILE}, )
SUBDIRS=tool
else
SUBDIRS=tool
endif

DEBUG_ENABLE = 1
ifeq (${DEBUG_ENABLE}, 1)
DEBUG = -g
else
DEBUG = -O2
endif

.PHONY: compile clean 

compile:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		cd ${PWD}/$$subdir && CROSS_COMPILE=$(CROSS_COMPILE) DEBUG=$(DEBUG) make || exit 1; \
	done;

clean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		cd ${PWD}/$$subdir && make clean || exit 1; \
	done;
