#!/bin/sh
# Updated 2026-08-26 (sdd-res-simbox Task 3.5):
# - ttyprog_programmator.c path fixed (sibling file now, not programmator/).
# - hub-ctrl build line removed: hub-ctrl.c now lives in the separate
#   libsCpp/asterisk-res-simbox-hub module with its own build
#   (see that module's build file), not built from here anymore.
gcc ttyprog_programmator.c -o /usr/simbox/programmator/programmator
