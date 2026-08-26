#!/bin/sh
# Path updated 2026-08-26 (sdd-res-simbox Task 3.5): ttyprog_programmator.c
# is now a sibling file in this module's src/ instead of a
# programmator/ subdirectory of the whole legacy tree.
gcc ttyprog_programmator.c -o /usr/simbox/programmator/programmator
