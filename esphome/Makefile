# Use this Makefile with target argument <cmd><cpuNum>
# <cmd> can be any valid esphome command such as:
#   'clean', 'compile', 'config', 'upload', or 'logs.
#
# Convenience <cmd> 'update' does a 'compile' and then an 'upload'.
#   (See 'esphome -h for more help on valid <cmds>)
#
# <cpuNum> is 0 or 1 or blank to issue <cmd> for both cpus
#
# Example usage: make compile0 or make update
#
ifeq ($(VIRTUAL_ENV),)
  $(error source esphome.git/venv/bin/activate)
endif

ifeq ($(USER),maarten)
  LOCATION = sxm
  SERIALNO = 100
else
  LOCATION = pec
  SERIALNO = 101
endif

ESPHOME   = esphome
GB_PREFIX = boot
GB_LOC    = location.$(LOCATION)

all:	 compile  
update0: compile0 upload0
update1: compile1 upload1
logs:
	@echo "Choose logs0 or logs1"

%: %0 %1
	@/usr/bin/true

define generateYaml
	cat $(GB_PREFIX)$(1).yaml | gcc -x c - -E -include $(GB_LOC)	\
		-D_CPU=$(1) -undef -nostdinc -D_SERIALNO=$(SERIALNO) |	\
		grep -v -x '[[:blank:]]*' > gen_$(GB_PREFIX)$(1).yaml
endef

%0:
	@$(call generateYaml,0)
	$(ESPHOME) `basename $@ 0` gen_$(GB_PREFIX)0.yaml

%1:
	@$(call generateYaml,1)
	$(ESPHOME) `basename $@ 1` gen_$(GB_PREFIX)1.yaml

Makefile: ;

clean: clean0 clean1
	@rm -f gen_$(GB_PREFIX)*.yaml .gen_$(GB_PREFIX)*.yaml

.PHONY: update0 update1 always-create clean all logs *0 *1
