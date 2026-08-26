# Copyright (C) 2014-2026 Anton Dodonov (NativeMind)
# https://github.com/Anton-Dodonov
# http://linkedin.com/in/anton-dodonov/
# mailto:anton.v.dodonov@gmail.com
#
# Written 2026-08-26 (sdd-res-simbox, Task 3.6/0.6). Simple, standalone
# Makefile -- see res_simbox_core's Makefile for the rationale.
#
# UNVERIFIED: no Asterisk build environment available in this development
# environment. Adjust ASTERISK_INCLUDE/DONGLE_CORE_INCLUDE before building.
#
# Two independent build targets, matching this module's two kinds of
# content (Requirements v1.1 / specs "Planned Module Layout"):
#   - $(MODULE): the real Asterisk module (cli_programmator.c, which
#     #include's ttyprog_svistok.c -> ttyprog_core.c -> crc.c). Depends on
#     res_simbox_core's headers (chan_dongle.h/cli.h/share.h) and links
#     against it at load time for gpublic/complete_device()/etc. -- a
#     one-directional dependency (programmator needs core; core does not
#     need programmator to load, per Requirements v1.1's core-standalone
#     constraint and Task 0.2's guard).
#   - $(TOOL): the standalone flashing CLI (ttyprog_programmator.c, which
#     #include's tty_v2.c, ttyprog_core.c, addons.c), same kind of
#     artifact as res_simbox_hub's hub-ctrl.c -- a manual operator tool,
#     not an Asterisk module (Task 3.4's finding).

MODULE = res_simbox_programmator.so
TOOL   = programmator
SRC    = src

ASTERISK_INCLUDE   ?= /usr/include
DONGLE_CORE_INCLUDE ?= ..

CC        = gcc
CFLAGS    = -fPIC -Wall -I$(SRC) -I$(ASTERISK_INCLUDE) -I$(DONGLE_CORE_INCLUDE)
LDFLAGS   = -shared
TOOL_CFLAGS = -Wall -I$(SRC)

MODULE_OBJS = $(SRC)/cli_programmator.o
TOOL_OBJS   = $(SRC)/ttyprog_programmator.o

all: $(MODULE) $(TOOL)

$(MODULE): $(MODULE_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(MODULE_OBJS)

$(TOOL): $(SRC)/ttyprog_programmator.c
	$(CC) $(TOOL_CFLAGS) -o $@ $<

$(SRC)/cli_programmator.o: $(SRC)/cli_programmator.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(MODULE_OBJS) $(MODULE) $(TOOL)

.PHONY: all clean
