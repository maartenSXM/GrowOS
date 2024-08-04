# GOS uses $(CPT_HOME)/cpptext to generate an esphome yaml
# using the C preprocessor, starting cpptext's starting point, main.yaml.
# Refer to ./main.yaml for what gets included by the C preprocessor.

GOS_HOME  ?= $(CURDIR)
GOS_BSP   ?= growboard
GOS_BUILD ?= gos
CPT_HOME  ?= $(GOS_BUILD)/cpptext
CPT_BUILD ?= $(GOS_BUILD)/$(GOS_BSP)

# .yaml files in these directories get dehashed into $(CPT_PROJDIR), which
# is defined in $(CPT_HOME)/cpptext/Makefile.cpptext.
CPT_DIRS  := . bsps/$(GOS_BSP) apps ui

# These files also are dehashed into $(CPT_PROJDIR)
CPT_FILES := partitions.csv

# These dirs trigger $(CPT_HOME)/cpptext/Makefile.esphome to rebuild 
CPT_ESP_DIRS  := utils libraries/libtelnet libraries/console

include $(CPT_HOME)/Makefile.cpptext

# GrowOS C pre-processor defines start with GOS_.  This reassigns
# the CPT_ defines from cpptext as GOS_ #defines for use in yaml and C.
CPT_EXTRA_CPPDEFS =				    \
		-I ./apps/			    \
		-I ./bsps/$(GOS_BSP)		    \
		-D GOS_HOME=$(GOS_HOME)		    \
		-D GOS_PROJDIR=$(CPT_PROJDIR)	    \
		-D GOS_PROJTAG=$(CPT_PROJTAG)	    \
		-D GOS_PROJTAG_$(CPT_PROJTAG)=1	    \
		-D GOS_USER=$(USER)		    \
		-D GOS_USER_$(USER)=1		    \
		-D GOS_BSP=$(GOS_BSP)		    \
		-D GOS_BSP_$(GOS_BSP)=1

$(CPT_PROJDIR)/partitions.csv: $(CPT_PROJDIR)/partitions.cpt
	@echo "Generating $@ from partitions.cpt"
	$(CPT_CPP) -MD -MP -MT $@ -MF $<.d $< -o $@

