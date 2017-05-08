#!/bin/bash

#
#   ls-gui-cvn - ls (list directory command) GUI by canvon
#   Copyright (C) 2017  Fabian Pietsch <fabian@canvon.de>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

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
