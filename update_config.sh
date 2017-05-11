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

[ "$#" -ge 2 ] || die "Usage: $BASENAME CONFIG_HEADER ACTION [ARGUMENTS ...]"

CONFIG_HEADER="$1"; shift
ACTION="$1"; shift

retrieve() {
	[ "$#" -eq 1 ] || die "Internal error: Usage: retrieve KEY"
	local KEY="$1"; shift

	[ -f "$CONFIG_HEADER" ] || {
		warn "retrieve: not a regular file: \"$CONFIG_HEADER\""
		return 3
	}

	local OLD_VALUE=$(sed -n -e "s/^#define $KEY \\(.*\\)/\\1/p" <"$CONFIG_HEADER") || {
		warn "retrieve: sed failed"
		return 2
	}

	[ -n "$OLD_VALUE" ] || { warn "retrieve: key \"$KEY\" not found"; return 1; }
	cat <<<"$OLD_VALUE"
}

exists() {
	[ "$#" -ge 1 ] || die "Internal error: Usage: exists KEY [KEY ...]"
	[ -f "$CONFIG_HEADER" ] || {
		#warn "exists: config header \"$CONFIG_HEADER\" does not exist or is not a regular file"
		return 1
	}

	local KEY
	for KEY in "$@"
	do
		grep -E -q "^#define $KEY " <"$CONFIG_HEADER" || return 1
	done
	return 0
}

was-updated() {
	echo "Updated config header \"$CONFIG_HEADER\" key \"$KEY\" to value \"$VALUE\""
	if [ -n "$MAKE" ]
	then
		if [ -z "$OK" ] || [ "$OK" -eq 0 ]
		then
			warn "Please re-run make so it will pick up the change to the file."
			return 3
		elif [ "$OK" -le 1 ]
		then
			warn "Let's hope make will pick up the change to the file. Otherwise, run make again."
			return 0
		else
			# (Silent, even under make.)
			return 0
		fi
	fi
}

update() {
	[ "$#" -eq 2 ] || die "Internal error: Usage: update KEY VALUE"
	local KEY="$1"; shift
	local VALUE="$1"; shift

	if ! [ -e "$CONFIG_HEADER" ]
	then
		echo "Creating config header \"$CONFIG_HEADER\""
		echo "#define $KEY $VALUE" >"$CONFIG_HEADER" || {
			warn "Couldn't create config header \"$CONFIG_HEADER\""
			return 1
		}
		OK=1 was-updated
		return
	fi

	if exists "$KEY"
	then
		local OLD_VALUE=$(retrieve "$KEY") || {
			warn "Error retrieving old value for key \"$KEY\" from config header \"$CONFIG_HEADER\": retrieve failed"
			return 1
		}

		# Old value equals new value? Then we should not update the file.
		[ "$OLD_VALUE" = "$VALUE" ] && return 0

		local VALUE_ESCAPED=$(sed -e 's#\\#\\\\#; s#/#\\/#' <<<"$VALUE") || {
			warn "Couldn't escape value \"$VALUE\" for sed"
			return 1
		}

		sed -i -e "s/^\\(#define $KEY\\) .*/\\1 ${VALUE_ESCAPED}/" "$CONFIG_HEADER" || {
			warn "Error write-processing config header \"$CONFIG_HEADER\""
			return 1
		}
		was-updated
		return
	else
		echo "#define $KEY $VALUE" >>"$CONFIG_HEADER" || {
			warn "Couldn't append to config header \"$CONFIG_HEADER\""
			return 1
		}
		was-updated
		return
	fi
}

update-capability-to-compile() {
	[ "$#" -gt 3 ] || die "Internal error: Usage: update-capability-to-compile MESSAGE DEFINE PROGRAM COMPILE [...]"
	local MSG="$1"; shift
	local DEF="$1"; shift
	local PROG="$1"; shift

	echo -n "Checking ${MSG}... "

	trap 'rm -f "tmp-$$.cc" "tmp-$$"' EXIT

	cat <<<"$PROG" >"tmp-$$.cc" || {
		warn "update-capability-to-compile: Cannot write test program"
		return 1
	}

	if "$@" "tmp-$$.cc" -o "tmp-$$" &>/dev/null && [ -f "tmp-$$" ]
	then
		RES=1
		echo "yes"
	else
		RES=0
		echo "no"
	fi
	update "$DEF" "$RES"
}

case "$ACTION" in
get|retrieve)
	[ "$#" -eq 1 ] || die "Usage: $BASENAME CONFIG_HEADER $ACTION KEY"
	KEY="$1"; shift

	#retrieve "$KEY" || die "Cannot retrieve config header \"$CONFIG_HEADER\" value for key \"$KEY\": retrieve failed"
	retrieve "$KEY"
	;;
exists)
	[ "$#" -ge 1 ] || die "Usage: $BASENAME CONFIG_HEADER $ACTION KEY [KEY ...]"

	exists "$@"
	;;
put|update)
	[ "$#" -eq 2 ] || die "Usage: $BASENAME CONFIG_HEADER $ACTION KEY VALUE"
	KEY="$1"; shift
	VALUE="$1"; shift

	#update "$KEY" "$VALUE" || die "Cannot update config header \"$CONFIG_HEADER\" value for key \"$KEY\" to \"$VALUE\": update failed"
	update "$KEY" "$VALUE"
	;;
update-capability-to-compile)
	[ "$#" -gt 3 ] || die "Usage: $BASENAME CONFIG_HEADER $ACTION MESSAGE DEFINE PROGRAM COMPILE [...]"
	MSG="$1"; shift
	DEF="$1"; shift
	PROG="$1"; shift

	update-capability-to-compile "$MSG" "$DEF" "$PROG" "$@"
	;;
*)
	die "Unrecognized action \"$ACTION\""
esac
