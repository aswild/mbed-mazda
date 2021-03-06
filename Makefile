# mbed LPC1768 and LPC1114 multi-platform makefile

# Default mbed-os SDK install path
MBED          ?= ./mbed

# Projects in this workspace
AVAILABLE_PROJECTS = mazda \
					 ISP

# Default Project
PROJECT           ?= mazda

# Ensure the project config exists, then include it
ifeq ($(wildcard $(PROJECT)/project.mk),)
$(error Fatal: Unable to find $(PROJECT)/project.mk)
endif
include $(PROJECT)/project.mk

# Ensure mbed.mk exists and include it
ifeq ($(wildcard $(MBED)/mbed.mk),)
$(error Fatal: Unable to find $(MBED)/mbed.mk)
endif
include $(MBED)/mbed.mk

# Auto targets so that "make project" is the same as "make PROJECT=project"
define project_rule_template
.PHONY: $(1)
$(1):
	$(MAKE) PROJECT=$(1) $(filter-out $(AVAILABLE_PROJECTS),$(MAKECMDGOALS))
endef
$(foreach project,$(AVAILABLE_PROJECTS),$(eval $(call project_rule_template,$(project))))
