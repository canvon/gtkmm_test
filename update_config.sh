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

was-updated() {
	echo "Updated config header \"$CONFIG_HEADER\" key \"$KEY\" to value \"$VALUE\"" >&2
	if [ -z "$OK" ] || [ "$OK" -eq 0 ]
	then
		warn "Please re-run make so it will pick up the change to the file."
		exit 3
	else
		warn "Let's hope make will pick up the change to the file. Otherwise, run make again."
		exit 0
	fi
}

if ! [ -e "$CONFIG_HEADER" ]
then
	echo "Creating config header \"$CONFIG_HEADER\""
	echo "#define $KEY $VALUE" >"$CONFIG_HEADER" || die "Couldn't create config header \"$CONFIG_HEADER\""
	OK=1 was-updated
fi

OLD_VALUE=$(sed -n -e "s/^#define $KEY \\(.*\\)/\\1/p" <"$CONFIG_HEADER") || die "Error read-processing config header \"$CONFIG_HEADER\""

# Old value equals new value? Then we should not update the file.
[ "$OLD_VALUE" = "$VALUE" ] && exit 0

if grep -E -q "^#define $KEY " <"$CONFIG_HEADER"
then
	VALUE_ESCAPED=$(sed -e 's#\\#\\\\#; s#/#\\/#' <<<"$VALUE") || die "Couldn't escape value \"$VALUE\" for sed"
	sed -i -e "s/^\\(#define $KEY\\) .*/\\1 ${VALUE_ESCAPED}/" "$CONFIG_HEADER" || die "Error write-processing config header \"$CONFIG_HEADER\""
	was-updated
else
	echo "#define $KEY $VALUE" >>"$CONFIG_HEADER" || die "Couldn't append to config header \"$CONFIG_HEADER\""
	was-updated
fi
exit 0
