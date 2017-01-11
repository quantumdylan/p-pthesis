#! /bin/bash

root -l -b <<EOF
.L ${1}+
.q
EOF
