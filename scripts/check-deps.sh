#!/bin/bash

set -o errexit
set -o nounset

if [[ $# != 1 ]]; then
    echo >&2 "usage: $0 <root-dir>"
    exit 1
fi

ROOT=$1
STATUS=0

for EXT in ext/*; do
    INNER=$(git submodule status --cached $EXT | egrep -o '[0-9a-f]{40}')
    OUTER=$( (cd $ROOT && git submodule status --cached $EXT) | egrep -o '[0-9a-f]{40}')
    if [[ $INNER != $OUTER ]]; then
        echo >&2 "$EXT: mismatch ($INNER vs. $OUTER)"
        STATUS=1
    fi
done
exit $STATUS
