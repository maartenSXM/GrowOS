FILE	= main.yaml
SRCS	= $(wildcard *.yaml)
CPU	= 0

DEFS = -I$(CPUDIR) -I. -D_USER_$(USER)=1 -D_CPU=$(CPU)

ifeq ($(USER),maarten)
  ifeq ($(CPU),0)
    logs jtag: CMDFLAGS = --device /dev/ttyUSB1
    ota:       CMDFLAGS = --device 192.168.8.134
  else
    logs jtag: CMDFLAGS = --device /dev/ttyUSB2
    ota:       CMDFLAGS = --device 192.168.8.135
  endif
endif

# Use this Makefile with a target command from this list:
# 		config compile upload logs update
# See 'esphome -h for help on esphome cmds. The default esphome
# command is 'compile'.
#
# Convenience commmand 'update' = 'compile' + 'upload'.
# Convenience commmand 'rebuild' = 'realclean' + 'compile'.
#
# Examples:
# 	make clean
# 	make update
#
# The yaml file $(FILE) defaults to main.yaml and can be set by
# editting it above or from the command line: make FILE=foo.yaml
# main.yaml is run through the Gnu C PreProcessor (CPREPROC) before
# running the resulting yaml through esphome. #include, #if, #else,
# #endif, # #define etc can be used in $(FILE) and files it includes.
#
# Also, /* */-style and //-style comments can be used in yaml with
# this approach. The resulting C-preprocessed yaml files are stored in
# directory $(CPUDIR). $(CPUDIR) defaults to cpu$(CPU).
# $(CPU) defaults to 0. 
#
# The file ./$(CPUDIR).yaml can be inspected if esphome indicates
# any specific line numbers have syntax errors. $(PROJECT) defaults to
# 'cpu0.yaml'.
# 
# A convenience define _USER_$(USER) is passed so that personal
# yaml can be written inside #if _USER_maarten / #endif blocks,
# if your userid is 'maarten'.
#
# Note that CPPROC directives in lambda functions may not work as
# expected since they will be processed earlier and not passed to
# esphome.  This has not been investigated, yet.
#
# Any target in this Makefile can be suffixed with -all so that it
# is run against all the CPUs every specified to this Makefile.
# e.g. 'make compile-all' or 'make clean-all'.

ifeq ($(VIRTUAL_ENV),)
  $(error You forgot to source esphome.git/venv/bin/activate)
endif

ESPHOME = esphome
CMDS	= config compile upload logs discover run
CPUDIR	= cpu$(CPU)
CPUS	= $(subst cpu,,$(wildcard cpu*/))
PROJECT	= $(CPUDIR).yaml

CPREPROC	= gcc -x c -undef -nostdinc $(DEFS) 
CPREPROC_FILES	= $(addprefix $(CPUDIR)/,$(filter-out $(wildcard cpu*.yaml),$(SRCS)))

# move $FILE to the end of the SRCS list
# YAMLS	= $(addprefix $CPUDIR)/,$(filter-out $(FILE),$(SRCS)) $(FILE))

all:	 compile  
run:	 update logs
update:  compile upload
jtag: 	 upload
ota: 	 upload
rebuild: espclean 
	make CPU=$(CPU) compile

$(CMDS): $(CPUDIR) $(CPREPROC_FILES) $(PROJECT)
	$(ESPHOME) $@ $(CMDFLAGS) $(PROJECT)

$(PROJECT): $(CPUDIR)/$(FILE)
	$(CPREPROC) -E -P -MD -MP -MT $@ -MF $<.d $< > $@

-include $(wildcard $(CPUDIR)/*.d)

%-all:
	@for cpu in $(CPUS:/=); do make CPU=$$cpu $(subst -all,,$@); done

realclean: 
	make clean-all
	rm -rf .esphome

espclean: $(CPUDIR) $(CPREPROC_FILES) $(PROJECT)
	$(ESPHOME) clean $(PROJECT)
	make CPU=$(CPU) clean

clean:
	rm -rf $(CPUDIR) $(PROJECT)

.PHONY: $(CMDS) update rebuild jtag ota espclean realclean all

# Strip yaml #-style comments lines and leave cpp directives

empty:=
space:= $(empty) $(empty)

.PRECIOUS: $(CPUDIR) $(CPREPROC_FILES) $(PROJECT)
$(CPUDIR)/%.yaml: %.yaml
	-@grep '$(subst $(space),,$(patsubst %,^[[:blank:]]*\#%\|,$(CPREPROC_DIRECTIVES)))^[[:blank:]]*[^# ]' $^ > $@

CPREPROC_DIRECTIVES = assert define elif else endif error ident if	\
	     ifdef ifndef import include include_next line		\
	     pragma sccs unassert undef warning

$(CPUDIR):
	-mkdir $@
