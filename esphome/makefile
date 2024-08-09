# This file, "./makefile", and "./Makefile" are based on examples from:
#
#       github.com/maartenSXM/cpptext
#
# This makefile installs cpptext from git, if necessary, and then builds bsps
MAKECMDGOALS ?= all

CPT_CLONE    := build
CPT_URL	     := git@github.com:maartenSXM/cpptext

# if $(CPT_CLONE)/cpptext does not exist, install cppttext there and restart

ifeq (,$(wildcard $(CPT_CLONE)/cpptext))
    ifeq (1,$(BAIL))
        $(error "$(firstword $(MAKEFILE_LIST)): Loop detected. Bailing out.")
    endif

$(MAKECMDGOALS): 
	mkdir -p $(CPT_CLONE)
	git -C $(CPT_CLONE) clone $(CPT_URL)
	$(MAKE) BAIL=1 --no-print-directory $(MAKECMDGOALS)
else

# if PRJ specifies a project Makefile, check it exists
ifneq (,$(PRJ))
  ifeq (,$(wildcard $(PRJ)))
    $(error makefile: $(PRJ) not found)
  endif
  PRJS := $(PRJ)
endif

# if PRJ is not specified, build all projects in ./projects
ifeq (,$(PRJS))
  PRJS ?= $(wildcard projects/*.mk)
endif

MK = $(MAKE) --no-print-directory
$(MAKECMDGOALS):
	@for prj in $(PRJS); do						    \
	   printf "makefile: invoking \"make $(MAKECMDGOALS)\" ";	    \
	   printf "for project `basename $$prj .mk`\n";			    \
	   $(MK) -C `dirname $$prj` -f `basename $$prj` $(MAKECMDGOALS);    \
	done
endif
