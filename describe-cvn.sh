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

[ "$#" -eq 2 ] || die "Usage: $BASENAME TAG_MATCH SUBDIR"

TAG_MATCH="$1"; shift
SUBDIR="$1"; shift

[ "$#" -eq 0 ] || die "Trailing arguments"

DESCRIBE=$(git describe --tags --dirty --match="$TAG_MATCH" --always) || die "git describe failed"

[ -n "$DESCRIBE" ] || die "git describe gave empty result"

if grep -E -q '^[0-9a-f]{7}(-dirty)?$' <<<"$DESCRIBE"
then
	warn "Looks like we got a fallback. Augmenting to look like the expected tag format..."
	COMMIT_HASH=$(git rev-list --max-count=1 --abbrev-commit HEAD "$SUBDIR") ||
	COMMIT_HASH=$(git rev-list --max-count=1 --abbrev-commit HEAD) || die "Cannot get most recent relevant commit's hash: git rev-list failed"

	TAG_BASE=${TAG_MATCH%\*}
	DESCRIBE="${TAG_BASE}unknown-g${COMMIT_HASH}"

	# Consider -dirty; limit it to SUBDIR, too.
	SUBDIR_STATUS=$(git status --porcelain "$SUBDIR") || die "Cannot determine work-tree dirty status: git status failed"
	[ -n "$SUBDIR_STATUS" ] && DESCRIBE="${DESCRIBE}-dirty"
elif grep -E -q -- '-[0-9]|-dirty$' <<<"$DESCRIBE"
then
	TAG=$(sed -e 's/\(.*\)-[0-9].*/\1/' <<<"$DESCRIBE") || die "Cannot get tag name from git describe output: sed failed"
	SUBDIR_COMMIT_COUNT=$(git rev-list --count "$TAG".. "$SUBDIR") || die "Cannot get count of commits since tag \"$TAG\" in subdir \"$SUBDIR\": git rev-list failed"
	[ -n "$SUBDIR_COMMIT_COUNT" ] || die "Cannot get count of commits since tag \"$TAG\" in subdir \"$SUBDIR\": git rev-list gave empty result"
	if [ "$SUBDIR_COMMIT_COUNT" -eq 0 ]
	then
		DESCRIBE="$TAG"
	else
		COMMIT_HASH=$(git rev-list --max-count=1 --abbrev-commit "$TAG".. "$SUBDIR") || die "Cannot get most recent relevant commit's hash: git rev-list failed"
		[ -n "$COMMIT_HASH" ] || die "Cannot get most recent relevant commit's hash: git rev-list gave empty result"
		DESCRIBE="${TAG}-${SUBDIR_COMMIT_COUNT}-g${COMMIT_HASH}"
	fi

	# Consider -dirty; limit it to SUBDIR, too.
	SUBDIR_STATUS=$(git status --porcelain "$SUBDIR") || die "Cannot determine work-tree dirty status: git status failed"
	[ -n "$SUBDIR_STATUS" ] && DESCRIBE="${DESCRIBE}-dirty"
fi

echo "$DESCRIBE"
