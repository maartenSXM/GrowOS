# "./makefile" and this file, "./Makefile", are based on examples from:
#
#       github.com/maartenSXM/cpptext
#
# "./makefile" automatically installs cpptext and then includes this file.
#
# Note: OUTDIR  is specified in "./makefile" and the default is "."
#       PROJDIR is specified in $(OUTDIR)/cpptext/Makefile.cpptext
#
# Lines changed from the example Makefile end with comment "# changed"

MAIN	:= main.yaml
SUFFIX	:= $(suffix $(MAIN))
DIRS	:= . ui							# changed
SRCS	:= $(foreach d,$(DIRS),$(wildcard $(d)/*$(SUFFIX)))	\
		    partitions.csv				# changed
PREFIX	:= myProj_
PROJTAG	:= 0

include $(OUTDIR)/cpptext/Makefile.cpptext

# uncomment the next line for esphome projects
include $(OUTDIR)/cpptext/Makefile.esphome		    # changed

$(PROJDIR)/partitions.csv: $(PROJDIR)/partitions.cpp
	@echo "Generating $@ from partitions.cpp"
	$(CPP) -MD -MP -MT $@ -MF $<.d $< > $@

