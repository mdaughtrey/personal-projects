#!/bin/bash

BIN=$1

MAIN_OFFSET=$(grep main ${BIN}.sym | cut -d' ' -f1)
BIN_OFFSET=$(grep "(code)" ${BIN}.sym | head -1 | cut -d' ' -f1)

./uploader ${BIN}.bin ${MAIN_OFFSET} ${BIN_OFFSET}
