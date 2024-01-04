FILE = boot.yaml
SRCS = $(wildcard *.yaml)
CPU  = 0

ifeq ($(USER),maarten)
  LOCATION = sxm
  SERIALNO = 100
else
  LOCATION = pec
  SERIALNO = 101
endif

DEFS = -D_USER_$(USER)=1 -D_SERIALNO=$(SERIALNO) -D_CPU=$(CPU)

# Use this Makefile with a target command from this list:
# 		config compile upload logs update
# See 'esphome -h for help on esphome cmds. The default esphome
# command is 'compile'.
#
# Convenience commmand 'update' = 'compile' + 'upload'.
#
# Examples:
# 	make clean
# 	make update
#
# The yaml file $(FILE) defaults to boot.yaml and can be set by
# editting it above or from the command line: make FILE=foo.yaml
# boot.yaml is run through the Gnu C PreProcessor (CPP) before running
# the resulting yaml through esphome. #include, #if, #else, #endif,
# #define etc can all be used in yaml files processed with CPP.
#
# Also, /* */-style and //-style comments can be used in yaml with
# this approach. The resulting C-preprocessed yaml is stored in
# file with suffix ".cpr". .cpr files are generated automatically.
#
# Remember to #include .cpr files, not .yaml files if they have
# #-style comments.
# The filie $(GEN) can be inspected if esphome indicates any specific
# line numbers have syntax errors.
# 
# A convenience define _USER_$(USER) is passed so that personal
# yaml can be written inside #if _USER_maarten / #endif blocks,
# if your userid is 'maarten'.
#
# Note that CPP directives in lambda functions may not work as
# expected since they will be processed earlier and not passed to
# esphome.  This has not been investigated, yet.

ifeq ($(VIRTUAL_ENV),)
  $(error You forgot to source esphome.git/venv/bin/activate)
endif

ESPHOME = esphome
CMDS	= config compile upload logs discover run
CPRS	= $(SRCS:.yaml=.cpr)
GEN	= $(FILE:.yaml=$(CPU).gen)
CPP	= gcc -x c -undef -nostdinc $(DEFS) -include location.$(LOCATION)

all:	 compile  
update:  compile upload
rebuild:
	rm -rf .esphome
	make clean
	make CPU=0
	make clean
	make CPU=1
	make clean

$(CMDS): $(CPRS) $(GEN)
	$(ESPHOME) $@ $(GEN)

$(GEN): $(FILE:.yaml=.cpr)
	$(CPP) -E -P -MD -MP -MT $@ $< > $@

-include $(SRCS:.yaml=.d)

realclean: $(GEN)
	$(ESPHOME) $@ $^
	rm -f *.d *.gen *.cpr

clean:
	rm -f *.d *.gen *.cpr

.PHONY: $(CMDS) update clean realclean all

# Strip yaml #-style comments lines and leave cpp directives

empty:=
space:= $(empty) $(empty)

.PRECIOUS: %.cpr
%.cpr: %.yaml
	-@grep '$(subst $(space),,$(patsubst %,^[[:blank:]]*\#%\|,$(CPP_DIRECTIVES)))^[[:blank:]]*[^# ]' $^ > $@

CPP_DIRECTIVES = assert define elif else endif error ident if	\
	     ifdef ifndef import include include_next line	\
	     pragma sccs unassert undef warning

