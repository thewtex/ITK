#!/usr/bin/env bash
set -o nounset
# set -o errexit

for F in *; do
  echo $F =====================
  diff $F ~/NrrdIO/$F
done