# Allow the user to compile with different base URLs for the DS and LO endpoints

ifneq ($(DS_BASE),)
$(info DS_BASE: $(DS_BASE))
CFLAGS += -DDS_BASE=\"$(DS_BASE)\"
ASFLAGS += -DDS_BASE=\"$(DS_BASE)\"
endif

ifneq ($(LO_BASE),)
$(info LO_BASE: $(LO_BASE))
CFLAGS += -DLO_BASE=\"$(LO_BASE)\"
ASFLAGS += -DLO_BASE=\"$(LO_BASE)\"
endif