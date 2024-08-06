# This file, "./makefile", and "./Makefile" are based on examples from:
#
#       github.com/maartenSXM/cpptext
#
# This makefile installs cpptext from git, if necessary, and then builds bsps

MAKEFLAGS    += --no-builtin-rules
MAKEFLAGS    += --no-builtin-variables
MAKECMDGOALS ?= all

GOS_BUILD    := build
CPT_CLONE    := $(GOS_BUILD)
CPT_URL	     := git@github.com:maartenSXM/cpptext

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
MAKEBSP := make -f Makefile --no-print-directory	
# build all BSPs found in ./bsps unless GOS_BSP aka BSP is pecified
ifneq (,$(GOS_BSP))
BSP := $(GOS_BSP)
else
BSP ?= $(filter-out bsps/common bsps/unsupported,$(wildcard bsps/*))
endif

$(MAKECMDGOALS):
	@for bsp in $(BSP); do					    \
	    printf "makefile: invoking \"make ";		    \
	    printf "GOS_BSP=`basename $$bsp` $(MAKECMDGOALS)\"\n";  \
	    $(MAKEBSP) GOS_BSP=`basename $$bsp` $(MAKECMDGOALS);    \
	done
endif
