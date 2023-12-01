ifeq ($(IDF_PATH),)
  $(error IDF_PATH is not set)
endif

IDFBUILD = idf.py build
IDFCLEAN = idf.py fullclean
IDFFLASH0 = idf.py -p /dev/cu.usbserial-101 -b 230400 flash
IDFFLASH1 = idf.py -p /dev/cu.usbserial-00000000 -b 230400 flash

.PHONY = clean clean0 clean1 build build0 build1 flash flash0 flash1 rebuild

build all: build0 build1
clean: clean0 clean1
flash: flash0 flash1
rebuild: clean build

build0:
	cd CPU0 && $(IDFBUILD)

build1:
	cd CPU1 && $(IDFBUILD)

flash0:
	cd CPU0 && $(IDFFLASH0)

flash1:
	cd CPU1 && $(IDFFLASH1)

clean0:
	cd CPU0 && $(IDFCLEAN)

clean1:
	cd CPU1 && $(IDFCLEAN)

