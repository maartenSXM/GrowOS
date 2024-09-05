# This should work for in-tree or out of tree GrowOS projects
ifeq (,$(GOS_HOME))
  $(error GOS_HOME not defined. Did you activate the GrowOS env?)
endif

TOP = $(shell realpath --relative-to=$(CURDIR)   $(GOS_HOME))
DIR = $(shell realpath --relative-to=$(GOS_HOME) $(CURDIR))
PRJ = debug.mk	# default project

MAKECMDGOALS ?= all
MAKE         := $(MAKE) -C $(TOP) PRJ=$(DIR)/$(PRJ) --no-print-directory

$(MAKECMDGOALS): 
	$(MAKE) $(MAKECMDGOALS)
