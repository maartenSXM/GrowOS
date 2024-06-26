# This Makefile was copied from cpptext/Makefile and modified to set
# variables. It clones cpptext so that its Makefile fragments can be
# included below.

MAIN    := main.yaml
PREFIX  := myProj_
PROJTAG := 0
SRCS    := $(wildcard *.yaml) $(wildcard pages/*.yaml)
OUTDIR  := .

-include $(OUTDIR)/cpptext/Makefile.cpptext
-include $(OUTDIR)/cpptext/Makefile.esphome

# if cpptext exists, git pull it - else git clone it
$(OUTDIR)/cpptext update:
	-@mkdir -p $(OUTDIR)
	-@if [ -d "$(OUTDIR)/cpptext" ]; then 				   \
		echo "Updating git repo $(OUTDIR)/cpptext";		   \
		cd $(OUTDIR)/cpptext; git pull; 			   \
	else								   \
		echo "Cloning git repo $(OUTDIR)/cpptext";		   \
		cd $(OUTDIR); git clone git@github.com:maartenSXM/cpptext; \
		@$(MAKE) --no-print-directory;				   \
	fi

.PHONY: update
