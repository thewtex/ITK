#!/usr/bin/env bash
#=============================================================================
# Copyright (c) 2010 Insight Software Consortium. All rights reserved.
# See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  See the above copyright notices for more information.
#=============================================================================


# This script makes optional suggestions for working with git.


if test "$(git config color.ui)" != "auto"; then
  cat << EOF

You may want to enable color output from Git commands with

  git config --global color.ui auto
  git config --global color.branch.current green
  git config --global color.branch.local   yellow
  git config --global color.branch.remote  red
  git config --global color.diff.meta      yellow
  git config --global color.grep.match     green
  git config --global color.status.changed yellow

EOF
fi

if ! bash -i -c 'echo $PS1' | grep -q '__git_ps1'; then
  cat << EOF

A dynamic, informative Git shell prompt can be obtained by sourcing the git
bash-completion script and setting the PS1 as suggested in the comments at the
top of that file.  You may need to install the bash-completion package from your
distribution to obtain the file.

EOF
fi

if ! git config merge.tool >/dev/null; then
  cat << EOF

A merge tool can be configured with

  git config merge.tool <toolname>

For more information, see

  git help mergetool

EOF
fi

echo "Done."
