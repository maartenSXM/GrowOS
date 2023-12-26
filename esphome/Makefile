# Use this Makefile with target argument <cmd><cpuNum>
# <cmd> can be any valid esphome command such as:
#   'clean', 'compile', 'config', 'upload', or 'logs.
# See 'esphome -h for more help on valid esphome cmds.
# <cpuNum> is 0 or 1 or blank to issue <cmd> for both cpus.
#
# Convenience <cmd> 'update' does a 'compile' and then an 'upload'.
#
# Examples:
# 	make compile0
# 	make update
#
ifeq ($(VIRTUAL_ENV),)
  $(error You forgot to source esphome.git/venv/bin/activate)
endif

ifeq ($(USER),maarten)
  LOCATION = sxm
  SERIALNO = 100
else
  LOCATION = pec
  SERIALNO = 101
endif

ESPHOME = esphome
GB_BOOT = boot
GB_LOC  = location.$(LOCATION)
GCC_CPP	= gcc -x c - -E -undef -nostdinc -include $(GB_LOC)
DEFINES	= -D_SERIALNO=$(SERIALNO) -D_USER_$(USER)=1

all:	 compile  
update0: compile0 upload0
update1: compile1 upload1
logs:
	@echo "Choose logs0 or logs1"

# The c-preprocessor is used to expand #defines & #ifs and also to remove
# C-style comments. The output is piped through grep to remove blank lines.
# This Makefile #defines _SERIALNO=<num>, _USER_<username>=1, and _CPU={0,1}
#
define genSystem
	$(GCC_CPP) $(DEFINES) -D_CPU=$(1) < $(GB_BOOT)$(1).yaml		\
	  | grep -v -x '[[:blank:]]*' > gen_$(GB_BOOT)$(1).yaml
endef

# target any verb to both cpus. e.g. compile becomes compile0 & compile1
%: %0 %1
	@/usr/bin/true

# strip the trailing 0 off any esphome verb and issue esphome
%0:
	@$(call genSystem,0)
	$(ESPHOME) `basename $@ 0` gen_$(GB_BOOT)0.yaml

# strip the trailing 1 off any esphome verb and issue it
%1:
	@$(call genSystem,1)
	$(ESPHOME) `basename $@ 1` gen_$(GB_BOOT)1.yaml

# don't let the default Makefile map to Makefile0 & Makefile1
Makefile: ;

# run esphome clean for both CPUs and also remove the generated yaml
clean: clean0 clean1
	@rm -f gen_$(GB_BOOT)*.yaml

.PHONY: update0 update1 always-create clean all logs *0 *1

