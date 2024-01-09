# This Makefile enables esphome projects to enjoy c-preprocessor
# directives and C-style comments in esphome YAML.
#
# Use this Makefile with a target esphome command from this list:
# 		config compile upload logs discover run
#
# See 'esphome -h for help on esphome cmds. The default esphome
# command is 'compile'.
#
# Convenience make target 'update'  = 'make compile' + 'make upload'.
# Convenience make target 'rebuild' = 'make realclean' + 'make compile'.
#
# Convenience make target 'clean' removes most generated yaml.
# Convenience make target 'espclean' does an 'esphome clean'.
# Convenience make target 'realclean' removes ./.esphome and generated yaml.
#
# Examples:
# 	make clean
# 	make update
#
# The yaml file $(MAIN) defaults to main.yaml and can be set by
# editting it below or from the command line: make MAIN=my_main.yaml.
#
# C-preprocessed yaml files are stored in directory $(PROJDIR).
# $(PROJDIR) defaults to proj$(PROJNUM).  $(PROJNUM) defaults to 0. 
#
# The file ./$(_MAIN).yaml can be inspected if esphome indicates
# any specific line numbers have syntax errors. $(_MAIN) defaults
# to 'proj$(PROJNUM).yaml'.
# 
# A convenience c-preprocessor define _USER_$(USER) is set so that
# personal yaml can be written inside, for example, #if _USER_maarten
# / #endif blocks, if your userid is 'maarten'.
#
# Any target in this Makefile can be suffixed with -all so that it
# is run against every PROJNUM currently specified to this Makefile.
# e.g. 'make compile-all' or 'make clean-all'.
#  

ifeq ($(VIRTUAL_ENV),)
  $(error You forgot to source esphome.git/venv/bin/activate)
endif

MAIN	= main.yaml
SRCS	= $(wildcard *.yaml)
PROJNUM	= 0

# setup default serial ports and IP addresses here, if desired.
# Then use make console/jtag/ota instead of make logs/upload/upload
# to bypass serial port selection
#
ifeq ($(USER),maarten)
  ifeq ($(PROJNUM),0)
    console: ESPDEV = --device /dev/ttyUSB1
    jtag:    ESPDEV = --device /dev/ttyUSB1
    ota:     ESPDEV = --device 192.168.8.134
  else
    console: ESPDEV = --device /dev/ttyUSB2
    jtag:    ESPDEV = --device /dev/ttyUSB2
    ota:     ESPDEV = --device 192.168.8.135
  endif
endif

ESPHOME = esphome
CMDS	= config compile upload logs discover run
PROJDIR	= proj$(PROJNUM)
PROJS	= $(subst proj,,$(wildcard proj*/))
_MAIN	= $(PROJDIR).yaml
CPP	= gcc -x c -undef -nostdinc $(DEFS) 
_YAMLS	= $(addprefix $(PROJDIR)/,$(filter-out $(wildcard proj*.yaml),$(SRCS)))
DEFS    = -I$(PROJDIR) -I. -D_USER_$(USER)=1 -D_PROJNUM=$(PROJNUM)

all:	 compile  
run:	 update console
update:  compile upload
console: logs
jtag: 	 upload
ota: 	 upload
rebuild: espclean 
	make PROJ=$(PROJNUM) compile

$(_MAIN) $(_YAMLS): Makefile

$(CMDS): $(PROJDIR) $(_YAMLS) $(_MAIN)
	$(ESPHOME) $@ $(ESPDEV) $(_MAIN)

$(_MAIN): $(PROJDIR)/$(MAIN)
	$(CPP) -E -P -MD -MP -MT $@ -MF $<.d $< > $@

$(PROJDIR):
	-mkdir $@

-include $(wildcard $(PROJDIR)/*.d)

%-all:
	@for prj in $(PROJS:/=); do make PROJNUM=$$prj $(subst -all,,$@); done

realclean: 
	make clean-all
	rm -rf .esphome

espclean: $(PROJDIR) $(_YAMLS) $(_MAIN)
	$(ESPHOME) clean $(_MAIN)
	make PROJNUM=$(PROJNUM) clean

clean:
	rm -rf $(PROJDIR) $(_MAIN)

.PHONY: $(CMDS) update rebuild console jtag ota espclean realclean all

# Strip yaml #-style comments lines and leave cpp directives

empty:=
space:= $(empty) $(empty)

.PRECIOUS: $(PROJDIR) $(_YAMLS) $(_MAIN)
$(PROJDIR)/%.yaml: %.yaml
	-@grep '$(subst $(space),,$(patsubst %,^[[:blank:]]*\#%\|,$(CPP_DIRECTIVES)))^[[:blank:]]*[^# ]' $< > $@

CPP_DIRECTIVES = assert define elif else endif error ident if	\
	     ifdef ifndef import include include_next line		\
	     pragma sccs unassert undef warning

