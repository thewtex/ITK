#!/usr/bin/env bash
#=============================================================================
# Copyright 2010 Kitware, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=============================================================================

# Run this script to set up the git hooks to commit changes to ITK.


die() {
	echo 'failure during hook setup' 1>&2
	echo '-------------------------' 1>&2
	echo '' 1>&2
	echo "$@" 1>&2
	exit 1
}


# Script can be executed from ${ITK_SOURCE}/, ${ITK_SOURCE}/Utilities, or
# ${ITK_SOURCE}/.git/hooks.

# We are in ${ITK_SOURCE}.
if test -d ./Utilities && test -d ./.git/hooks; then
  cd .git/hooks || die "Could not cd to hook directory."
# We are in ${ITK_SOURCE}/Utilities
elif test -d ../.git/hooks; then
  cd ../.git/hooks || die "Could not cd to hook directory."
# We are in ${ITK_SOURCE}/.git/hooks
elif test -d ../hooks && test -d ../../.git; then
  cd .
else
  die 'Could not cd to hook directory.  This script must be executed from either
${ITK_SOURCE}/, ${ITK_SOURCE}/Utilities, or ${ITK_SOURCE}/.git/hooks.'
fi


# We need to have a git repository to do a pull.
if ! test -d ./.git; then
  git init || die "Could not run \'git init\'."
fi


# Grab the hooks.
cd ../..
# Use the local hooks if possible.
if git branch -r | grep -q 'origin/hooks$'; then
  cd .git/hooks
  git pull .. remotes/origin/hooks
else
  cd .git/hooks
  git pull git://public.kitware.com/ITK.git hooks || die "Downloading the hooks failed."
fi
cd ../..


# Set up uncrustify hook.
git config hooks.uncrustify.conf "${PWD}/Utilities/uncrustify.conf"


# Set up KWStyle hook.
git config hooks.KWStyle.conf "${PWD}/Utilities/KWStyle/ITK.kws.xml.in"
git config hooks.KWStyle.overwriteRulesConf "${PWD}/Utilities/KWStyle/ITKOverwrite.txt"
