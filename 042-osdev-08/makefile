BASE_DIRECTORY := $(shell pwd)

export BASE_DIRECTORY

include $(BASE_DIRECTORY)/make/macros.mk

directories := source

include $(BASE_DIRECTORY)/make/recurse.mk

clean:
	$(noecho)exec $(MAKE) --no-print-directory --directory=build $(MAKECMDGOALS)

build:
	$(noecho)exec $(MAKE) --no-print-directory --directory=build $(MAKECMDGOALS)