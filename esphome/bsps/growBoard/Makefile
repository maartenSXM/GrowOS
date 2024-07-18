# "./makefile" and this file, "./Makefile", are based on examples from:
#
#       github.com/maartenSXM/cpptext
#
# "./makefile" automatically installs cpptext and then includes this file.
#
# Note: OUTDIR  is specified in "./makefile" and the default is "."

DIRS	:= . ui
ESPDIRS := utils libraries/libtelnet libraries/console

MAIN	:= main.yaml
SUFFIX	:= $(suffix $(MAIN))
PREFIX	:= myProj_
PROJTAG	:= 0

SRCS	:= $(foreach d,$(DIRS),$(wildcard $(d)/*$(SUFFIX))) partitions.csv
ESPDEPS := $(foreach d,$(ESPDIRS),$(wildcard $(d)/*.c) $(wildcard $(d)/*.h))

include $(OUTDIR)/cpptext/Makefile.cpptext

# GrowOS C pre-processor defines start with GOS_
EXTRA_CPPDEFS = -D GOS_PROJDIR=$(PROJDIR)	\
		-D GOS_PROJTAG=$(PROJTAG)	\
		-D GOS_PROJTAG_$(PROJTAG)=1	\
		-D GOS_USER=$(USER)		\
		-D GOS_USER_$(USER)=1

# PROJDIR is defined in $(OUTDIR)/cpptext/Makefile.cpptext
$(PROJDIR)/partitions.csv: $(PROJDIR)/partitions.cpp
	@echo "Generating $@ from partitions.cpp"
	@$(CPP) -MD -MP -MT $@ -MF $<.d $< > $@

