GOS_BSP	?= growBoard

CPT_DIRS  := . ui
CPT_FILES := partitions.csv

# Declare C/C++ code directories that trigger Makefile.esphome to rebuild firmware
ESP_DIRS  := utils libraries/libtelnet libraries/console

include $(OUTDIR)/cpptext/Makefile.cpptext

# GrowOS C pre-processor defines start with GOS_
EXTRA_CPPDEFS = -I bsps/$(GOS_BSP)		\
		-D GOS_PROJDIR=$(PROJDIR)	\
		-D GOS_PROJTAG=$(PROJTAG)	\
		-D GOS_PROJTAG_$(PROJTAG)=1	\
		-D GOS_USER=$(USER)		\
		-D GOS_USER_$(USER)=1		\
		-D GOS_BSP=$(GOS_BSP)		\
		-D GOS_BSP_$(GOS_BSP)=1

# PROJDIR is defined in $(OUTDIR)/cpptext/Makefile.cpptext
$(PROJDIR)/partitions.csv: $(PROJDIR)/partitions.cpp
	@echo "Generating $@ from partitions.cpp"
	$(CPP) -MD -MP -MT $@ -MF $<.d $< -o $@

