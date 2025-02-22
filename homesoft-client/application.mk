ifeq ($(DEBUG),true)
    $(info >Starting application.mk)
endif

ifeq ($(ENABLE_BOOTSEL),1)
CFLAGS += -DENABLE_BOOTSEL
endif

