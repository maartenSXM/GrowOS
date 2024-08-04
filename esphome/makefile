# This file, "./makefile", and "./Makefile" are based on examples from:
#
#       github.com/maartenSXM/cpptext
#
# This makefile installs cpptext from git, if necessary, and then builds bsps

MAKEFLAGS    += --no-builtin-rules
MAKEFLAGS    += --no-builtin-variables
MAKECMDGOALS ?= all

GOS_BUILD    := gos
CPT_CLONE    := $(GOS_BUILD)
CPT_URL	     :=git@github.com:maartenSXM/cpptext

# install cppttext into $(GOS_BUILD) if it is not there, and then restart
ifeq (,$(wildcard $(CPT_CLONE)/cpptext))
    ifeq (1,$(BAIL))
        $(error "$(firstword $(MAKEFILE_LIST)): Loop detected. Bailing out.")
    endif

$(MAKECMDGOALS): 
	mkdir -p $(CPT_CLONE)
	git -C $(CPT_CLONE) clone $(CPT_URL)
	$(MAKE) BAIL=1 --no-print-directory $(MAKECMDGOALS)
else
# build all BSPs found in ./bsps except in ./bsps/legacy, unless BSP specified
BSP     ?= $(filter-out bsps/legacy,$(wildcard bsps/*))
MAKEBSP := make -f Makefile --no-print-directory	    \
	    GOS_BUILD=$(GOS_BUILD) CPT_HOME=$(CPT_CLONE)/cpptext

$(MAKECMDGOALS):
	@for bsp in $(BSP); do                                      \
	    printf "makefile: building $$bsp with: make $(MAKECMDGOALS) \n"; \
	    $(MAKEBSP) GOS_BSP=`basename $$bsp` $(MAKECMDGOALS);    \
	done
endif
