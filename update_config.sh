#!/bin/bash

warn() {
	echo "$BASENAME: $*" >&2
}

die() {
	warn "Fatal: $*"
	exit 1
}

BASENAME="${0##*/}"

[ "$#" -ge 3 ] || die "Usage: $BASENAME CONFIG_HEADER KEY VALUE"

CONFIG_HEADER="$1"; shift
KEY="$1"; shift
VALUE="$1"; shift

[ "$#" -eq 0 ] || die "Trailing arguments"

if ! [ -e "$CONFIG_HEADER" ]
then
	echo "Creating config header \"$CONFIG_HEADER\""
	echo "#define $KEY $VALUE" >"$CONFIG_HEADER" || die "Couldn't create config header \"$CONFIG_HEADER\""
	exit 0
fi

OLD_VALUE=$(sed -n -e "s/^#define $KEY \\(.*\\)/\\1/p" <"$CONFIG_HEADER") || die "Error read-processing config header \"$CONFIG_HEADER\""

# Old value equals new value? Then we should not update the file.
[ "$OLD_VALUE" = "$VALUE" ] && exit 0

if grep -E -q "^#define $KEY " <"$CONFIG_HEADER"
then
	sed -i -e "s/^\\(#define $KEY\\) .*/\\1 $VALUE/" "$CONFIG_HEADER" || die "Error write-processing config header \"$CONFIG_HEADER\""
else
	echo "#define $KEY $VALUE" >>"$CONFIG_HEADER" || die "Couldn't append to config header \"$CONFIG_HEADER\""
fi
exit 0
