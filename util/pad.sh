#!/bin/bash

PAD="$1"; shift
FILE="$1"; shift

[ -f "$FILE" ] || { echo "File $FILE not found"; exit 1; }
[ "$PAD" -gt 1 ] || { echo "Invalid pad size $PAD"; exit 1; }

size() {
    stat -f %z $1
}

cursize=$(size $FILE)
newsize=$(( ($b - 1) / $PAD * $PAD + $PAD))

if [ "$newsize" -gt "$cursize" ]; then
    head -c $((newsize - cursize)) /dev/zero >> $FILE
    exit 0
fi

exit 1
