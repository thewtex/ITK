#!/usr/bin/env bash
#=============================================================================
# Copyright (c) 2010 Insight Software Consortium. All rights reserved.
# See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  See the above copyright notices for more information.
#=============================================================================

# Run this script to set up the git hooks to commit changes to ITK.


die() {
	echo 'failure during hook setup' 1>&2
	echo '-------------------------' 1>&2
	echo '' 1>&2
	echo "$@" 1>&2
	exit 1
}


u=$(echo "$0"|sed 's/[^/]*$//')
cd "$u/../.git/hooks"

# We need to have a git repository to do a pull.
if ! test -d ./.git; then
  git init || die "Could not run git init."
fi


# Grab the hooks.
cd ../..
# Use the local hooks if possible.
if git for-each-ref refs/remotes/origin/hooks | grep -q 'origin/hooks'; then
  cd .git/hooks
  git pull .. remotes/origin/hooks
else
  cd .git/hooks
  git pull http://public.kitware.com/ITK.git hooks || die "Downloading the hooks failed."
fi
cd ../..


# Set up uncrustify hook.
git config hooks.uncrustify.conf "Utilities/uncrustify.conf"


# Set up KWStyle hook.
git config hooks.KWStyle.conf "Utilities/KWStyle/ITK.kws.xml.in"
git config hooks.KWStyle.overwriteRulesConf "Utilities/KWStyle/ITKOverwrite.txt"
