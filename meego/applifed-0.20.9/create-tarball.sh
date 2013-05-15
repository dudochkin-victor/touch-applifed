#!/bin/sh
if test -n "$1"
then
    git archive --format tar --remote=$1 --output=meegotouch-applifed-0.20.9.tar --prefix meegotouch-applifed-0.20.9/ released/0.20.9
    bzip2 meegotouch-applifed-0.20.9.tar
else
    echo "Usage: create-tarball.sh <repo>"
    echo "       where <repo> is suitable for git archive --remote=<repo>"
fi
