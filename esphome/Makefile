# Copy this Makefile to any project that you want to use it with.
# Then, the first time make is run, it will install itself under OUTDIR.
#
# This Makefile is based on a template from github.com/maartenSXM/cpptext 
# The only four changes are tagged with # ***		    # ***

MAIN	:= main.yaml
SUFFIX	:= $(suffix $(MAIN))
DIRS	:= . ui						    # ***
SRCS	:= $(foreach d,$(DIRS),$(wildcard $(d)/*$(SUFFIX))) # ***
OUTDIR	:= .
PREFIX	:= myProj_
PROJTAG	:= 0

-include $(OUTDIR)/cpptext/Makefile.cpptext

# uncomment the next line for esphome projects
-include $(OUTDIR)/cpptext/Makefile.esphome		    # ***

$(OUTDIR)/cpptext: 
	-@mkdir -p $(OUTDIR)
	git -C $(OUTDIR) clone git@github.com:maartenSXM/cpptext
	$(MAKE) --no-print-directory
