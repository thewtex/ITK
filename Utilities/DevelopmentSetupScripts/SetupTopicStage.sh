#!/usr/bin/env bash
#=============================================================================
# Copyright (c) 2010 Insight Software Consortium. All rights reserved.
# See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  See the above copyright notices for more information.
#=============================================================================


# Run this script to set up the topic stage for pushing changes.

die() {
	echo 'failure during topic stage setup' 1>&2
	echo '--------------------------------' 1>&2
	echo '' 1>&2
	echo "$@" 1>&2
	exit 1
}

# Make sure we are inside the repository.
cd "$(echo "$0"|sed 's/[^/]*$//')"

if git config remote.stage.url >/dev/null; then
  echo "Topic stage remote was already configured."
else
  echo "Configuring the topic stage remote..."
  git remote add stage http://itk.org/stage/ITK.git || \
    die "Could not add the topic stage remote."
  git config remote.stage.pushurl git@itk.org:stage/ITK.git
fi

if test ! -f ~/.ssh/identity -a ! -f ~/.ssh/id_dsa -a ! -f ~/.ssh/id_rsa; then
  cat << EOF

ssh key pair not detected.  This is needed for developers with push access to
the main repository.  In order to generate a key pair, run

  ssh-keygen

To keep a single public key on file with the server across multiple machines,
add the following to ~/.ssh/config

  Host "itk.org"
      IdentityFile=~/.ssh/id_git_itk

and copy the appropriate ~/.ssh/id_rsa.pub to ~/.ssh/id_git_itk.
EOF
fi

echo "Done."
