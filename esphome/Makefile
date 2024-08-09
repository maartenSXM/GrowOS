# GOS uses cpptext to generate an esphome yaml file from gos/main.yaml
# using the C preprocessor. Refer to ./makefile for how cpptext is installed.

MAKEFILE     := $(abspath $(lastword $(MAKEFILE_LIST)))

# defines used by GOS
GOS_HOME     := ../..
GOS_PROJECT  ?= gosProject
GOS_BSP      ?= bsps/growBoardCpu0
GOS_CONFIG   ?= configs/growBoard0

# defines used by $(CPT_HOME)/Makefile.cpptext
CPT_HOME        := build/cpptext
CPT_BUILD_DIR   := build/$(GOS_PROJECT)
CPT_TMP_SUBDIR  := dehashed
CPT_GEN         := init.yaml partitions.csv

# The next two are defines used by $(CPT_HOME)/Makefile.esphome.
# CPT_ESP_INIT is C preprocessed to create a full esphome yaml file.
# CPT_ESP_YAML is the output of $(CPT_HOME)/yamlmerge.sh which has
# all the esphome component definitions merged together to form
# legitimate yaml that esphome tools can parse.

CPT_ESP_INIT := init.yaml
CPT_ESP_YAML := esphome.yaml

# esphome yaml files in these directories will get dehashed by cpptext .

_DIRS         := gos apps ui bsps/common bsps/$(GOS_BSP)
CPT_SRCS     := $(foreach d,$(_DIRS),$(wildcard $(d)/*.yaml)) $(CPT_GEN)

# These c/c++ files trigger a $(CPT_HOME)/cpptext/Makefile.esphome 

_DEPS := utils libraries/libtelnet libraries/console
CPT_ESP_DEPS := $(foreach d,$(_DEPS), $(wildcard $(d)/*.c)	\
		$(wildcard $(d)/*.cpp) $(wildcard $(d)/*.h))

# GOS_HOME is two levels up from the generated esphome.yaml because
# CPT_BUILD_DIR is build/$(GOS_PROJECT) which is two levels down from here

CPT_EXTRA_INCS := -I ./bsps/$(GOS_BSP)
CPT_EXTRA_DEFS := -D GOS_HOME=../..				\
		  -D GOS_PROJDIR=$(CPT_BUILD_DIR)		\
		  -D GOS_PROJECT=$(GOS_PROJECT)			\
		  -D GOS_PROJECT_$(GOS_PROJECT)=1		\
		  -D GOS_CONFIG=$(GOS_CONFIG)			\
		  -D GOS_CONFIG_$(notdir $(GOS_CONFIG))=1	\
		  -D GOS_BSP=$(GOS_BSP)				\
		  -D GOS_BSP_$(notdir $(GOS_BSP))=1		\
		  -D GOS_USER=$(USER)				\
		  -D GOS_USER_$(USER)=1

include $(CPT_HOME)/Makefile.cpptext

