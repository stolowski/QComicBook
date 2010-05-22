#!/bin/sh
#
# This script updates/creates translation (.ts) files.
# To create new translation, execute:
#   ./update_ts.sh qcomicbook_xx_XX.ts
# (replace xx_XX with your country code).
#
# Must be executed in lang subdirectory.
#
lupdate ../src -ts *.ts $@
