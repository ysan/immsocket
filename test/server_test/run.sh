#!/bin/bash

BASE_DIR=${HOME}/prog/immsocket


export LD_LIBRARY_PATH=${BASE_DIR}/immsocket:${BASE_DIR}/immsocketcommon:${BASE_DIR}/immsocketservice

${BASE_DIR}/test/server_test/server_test

