#---------------------------------------------------------------------------------------------------
#
#	By Karol Grzybowski, 2017
#
#---------------------------------------------------------------------------------------------------

ifneq ($(directories),)

#
# Recurse over specified list of subdirectories.
#

.PHONY: $(directories) recurse

$(MAKECMDGOALS) recurse: $(directories)

#
# Specify target for directories.
#
$(directories):
	$(noecho)exec $(MAKE) --no-print-directory --directory=$(@) --makefile=dirs.mk $(MAKECMDGOALS)

endif

#---------------------------------------------------------------------------------------------------

ifneq ($(projects),)

#
# Recurse over specified list of directories containing project.mk files.
#

.PHONY: $(projects) recurse

$(MAKECMDGOALS) recurse: $(projects)

#
# Specify target for project.mk files.
#
$(projects):
	$(noecho)exec $(MAKE) --no-print-directory --directory=$(@) --makefile=project.mk $(MAKECMDGOALS) --jobs $(shell nproc)

endif