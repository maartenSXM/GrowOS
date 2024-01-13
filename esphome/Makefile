# This Makefile enables esphome projects to enjoy c-preprocessor
# directives and C-style comments in esphome YAML.
#
# Use this Makefile with a target esphome command from this list:
# config, compile, upload, logs, discover and run.
#
# See 'esphome -h for help on esphome cmds. The default esphome
# command is 'compile'.
#
# Convenience make target 'clean' removes most generated yaml.
# Convenience make target 'espclean' does an 'esphome clean'.
# Convenience make target 'realclean' removes ./.esphome and generated yaml.
# Convenience make target 'rebuild' = 'make realclean' + 'make compile'.
#
# Examples:
# 	make config
# 	make compile
# 	make upload
#
# The yaml file $(MAIN) defaults to main.yaml and can be set by
# editting it below or from the command line: make MAIN=my_main.yaml
#
# C-preprocessed yaml files are stored in directory $(PROJDIR).
# $(PROJDIR) defaults to proj.$(PROJTAG). $(PROJTAG) defaults to 0. 
# C-preprocessor ifdefs can be used to generate many different esphome
# projects from the same directory. It is possible to use multiple
# PROJTAGs to generate multiple esphome projects them from the same
# yaml files.  See the example in this directory, especially ./config.h
#
# The file ./$(_MAIN).yaml can be inspected if esphome indicates
# any specific line numbers have syntax errors. $(_MAIN) defaults
# to 'proj.$(PROJTAG).yaml'.
# 
# A convenience c-preprocessor define _USER_$(USER) is set so that
# personal yaml can be written inside, for example, #if _USER_maarten
# / #endif blocks, if your userid is 'maarten'.
#
# Any target in this Makefile can be suffixed with -all so that it
# is run against every PROJTAG currently specified to this Makefile.
# e.g. 'make compile-all' or 'make clean-all'.
#  

MAIN	= main.yaml
SRCS	= $(wildcard *.yaml)
PROJTAG	= 0

# Setup default serial ports and IP addresses here, if desired.
# Then use make console/jtag/ota instead of make logs/upload/upload
# to bypass serial port selection. Also, once IP addresses are set
# here correctly, convenience make target 'update' can be used and
# it performs a 'make compile' + 'make ota'.
#
ifneq ($(USER),maarten)
  # default values for all usernames
  console: ESPDEV = --device /dev/ttyUSB1
  jtag:    ESPDEV = --device /dev/ttyUSB1
  ota:     ESPDEV = --device 192.168.1.2
else
  # default values for username 'maarten'
  ifeq ($(PROJTAG),0)
    console: ESPDEV = --device /dev/ttyUSB1
    jtag:    ESPDEV = --device /dev/ttyUSB1
    ota:     ESPDEV = --device 192.168.8.134
  else
    console: ESPDEV = --device /dev/ttyUSB2
    jtag:    ESPDEV = --device /dev/ttyUSB2
    ota:     ESPDEV = --device 192.168.8.135
  endif
endif

# check that an esphome virtual environment is setup
ifeq ($(VIRTUAL_ENV),)
  $(waring Did you forgot to source esphome.git/venv/bin/activate?)
endif

# the make rules below require yaml files to have .yaml suffixes.
ifneq (x$(suffix $(MAIN)),x.yaml)
  $(error "Suffix of MAIN yaml file must be .yaml")
endif

ESPHOME = esphome
CMDS	= config compile upload logs discover run
PROJDIR	= proj.$(PROJTAG)
PROJTAGS= $(subst proj.,,$(wildcard proj.*/))
_MAIN	= $(PROJDIR).yaml
CPP	= gcc -x c -undef -nostdinc $(DEFS) 
_YAMLS	= $(addprefix $(PROJDIR)/,$(filter-out $(wildcard proj.*.yaml),$(SRCS)))
DEFS    = -I$(PROJDIR) -I. -D_PROJTAG_$(PROJTAG)=1 -D_USER_$(USER)=1

all:	 compile  
run:	 update console
update:  compile ota
console: logs
jtag: 	 upload
ota: 	 upload
rebuild: espclean 
	@make --no-print-directory PROJTAG=$(PROJTAG) compile

$(_MAIN) $(_YAMLS): Makefile

$(CMDS): $(PROJDIR) $(_YAMLS) $(_MAIN)
	-$(ESPHOME) $@ $(ESPDEV) $(_MAIN)

$(_MAIN): $(PROJDIR)/$(MAIN)
	$(CPP) -E -P -MD -MP -MT $@ -MF $<.d $< > $@

$(PROJDIR): FORCE
	-mkdir -p $@
	find . -type d \( -path "./proj.*" -o -path "./.esphome"\) -prune -print0 | tar cf - --null --files-from - --no-recursion| tar xf - -C $@

FORCE: ;


-include $(wildcard $(PROJDIR)/*.d)

%-all:
	@for prj in $(PROJTAGS:/=); do make --no-print-directory PROJTAG=$$prj $(subst -all,,$@); done

realclean: 
	make --no-print-directory clean-all
	rm -rf .esphome

espclean: $(PROJDIR) $(_YAMLS) $(_MAIN)
	$(ESPHOME) clean $(_MAIN)
	make --no-print-directory PROJTAG=$(PROJTAG) clean

clean:
	rm -rf $(PROJDIR) $(_MAIN)

.PRECIOUS: $(PROJDIR) $(_YAMLS) $(_MAIN)
.PHONY: $(CMDS) update rebuild console jtag ota espclean realclean all test

# Strip yaml #-style comments lines and leave cpp directives, storing
# yaml files with cpp directives in $(PROJDIR).

empty:=
space:= $(empty) $(empty)

$(PROJDIR)/%.yaml: %.yaml
	-@grep '$(subst $(space),,$(patsubst %,^[[:blank:]]*\#%\|,$(CPP_DIRECTIVES)))^[[:blank:]]*[^# ]' $< > $@

CPP_DIRECTIVES = assert define elif else endif error ident if	\
	     ifdef ifndef import include include_next line	\
	     pragma sccs unassert undef warning

tests:
	@cd test; make --no-print-directory test
