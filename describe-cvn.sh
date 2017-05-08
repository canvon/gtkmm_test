#!/bin/bash

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

DESCRIBE=$(git describe --tags --dirty --match="$TAG_MATCH") || die "git describe failed"

[ -n "$DESCRIBE" ] || die "git describe gave empty result"

if grep -E -q -- '-[0-9]|-dirty$' <<<"$DESCRIBE"
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
