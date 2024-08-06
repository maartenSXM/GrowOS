# GOS uses cpptext to generate an esphome yaml file from gos/main.yaml
# using the C preprocessor. Refer to ./makefile for how cpptext is installed.

MAKEFILE     := $(abspath $(lastword $(MAKEFILE_LIST)))

GOS_HOME     := ../..

# GOS_HOME is two levels up from generated esphome.yaml because CPT_BUILD build
# is two levels down from this directory. To use absolute paths, uncomment this:
# GOS_HOME   := $(patsubst %/,%,$(dir $(MAKEFILE)))

GOS_BSP      ?= growBoardCpu0
GOS_BUILD    ?= build
GOS_PROJECT  ?= gosProject

CPT_HOME     := build/cpptext
CPT_BUILD    := build/$(GOS_PROJECT)
CPT_PROJECT  := dehashed
CPT_GEN      := esphome.yaml partitions.csv
CPT_ESP_YAML := esphome.yaml

_DIRS         := gos apps ui bsps/common bsps/$(GOS_BSP)
CPT_SRCS     := $(foreach d,$(_DIRS),$(wildcard $(d)/*.yaml)) $(CPT_GEN)

_DEPS := utils libraries/libtelnet libraries/console

# These c/c++ files trigger a $(CPT_HOME)/cpptext/Makefile.esphome 
CPT_ESP_DEPS := $(foreach d,$(_DEPS), $(wildcard $(d)/*.c)	\
		$(wildcard $(d)/*.cpp) $(wildcard $(d)/*.h))

CPT_EXTRA_INCS := -I ./bsps/$(GOS_BSP)
CPT_EXTRA_DEFS := -D GOS_HOME=$(GOS_HOME)		    \
		  -D GOS_PROJDIR=$(CPT_BUILD)		    \
		  -D GOS_PROJECT=$(GOS_PROJECT)		    \
		  -D GOS_PROJECT_$(GOS_PROJECT)=1	    \
		  -D GOS_USER=$(USER)			    \
		  -D GOS_USER_$(USER)=1			    \
		  -D GOS_BSP=$(GOS_BSP)			    \
		  -D GOS_BSP_$(GOS_BSP)=1

include $(CPT_HOME)/Makefile.cpptext

