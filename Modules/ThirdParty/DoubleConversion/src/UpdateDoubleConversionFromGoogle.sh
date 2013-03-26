#!/bin/sh
#
# UpdateDoubleConversionFromGoogle.sh
#
# This script automates the patching process to merge
# in changes from the Google Code repository for double-conversion.
# It is based on work by Brad King that is used to update VNL
#
# See /scratch/kent/itktest/ITK/Modules/ThirdParty/VNL/src/README-ITK.txt
# for more commentary on how this update process works.
#
echo "-------------------------------------------------------"
echo This script will update source code for the double-conversion
echo library from http://code.google.com
echo "-------------------------------------------------------"
if [ ! -f Modules/ThirdParty/DoubleConversion/src/README-ITK.txt ]
then
    echo The current working directory $(pwd) is not the top level
    echo of the ITK source code tree. Please change to the
    echo ITK source directory and re-run this script
    exit 1
fi
#
# Once the merge has been done
# EDIT THIS SCRIPT to change the hash tag at which to begin the
# next update...
git branch DoubleConversion-upstream 10d4e478188877348e6384788ca067c4985ab76b
#
# Make a temp directory to handle the import of the upstream source
mkdir DoubleConversion-Tmp
cd DoubleConversion-Tmp
#
# base a temporary git repo off of the parent ITK directory
git init
#
# pull in the upstream branch
git pull .. DoubleConversion-upstream
#
# empty out all existing source
rm -rf *
#
# download and copy the necessary double-conversion source
echo Cloning upstream HEAD from code.google.com
echo NOTE: to check out a particular revision for merging
echo you have to add a git checkout '<hash>'
echo or git checkout '<branchname>'
echo after the git clone command
git clone https://code.google.com/p/double-conversion/

cp -r double-conversion/src/* .
# get rid of double-conversion clone
rm -fr double-conversion
#
# add upstream files in Git
git add -u
git add .
#
# commit new source
GIT_AUTHOR_NAME='Google double-conversion Maintainers' \
GIT_AUTHOR_EMAIL='floitsch@google.com' \
GIT_AUTHOR_DATE="$(git log -1 | grep '^Date:' | sed -e 's/^Date: *//')" \
git commit -q -m "Google double-conversion (reduced)

Change-Id: I$(git rev-parse HEAD)"
#
# push to the DoubleConversion-upstream branch in the
# ITK tree
git push .. HEAD:DoubleConversion-upstream
cd ..
#
# get rid of temporary repository
rm -fr DoubleConversion-tmp
#
# checkout a new update branch off of the master.
git checkout -b DoubleConversion-update master
#
# use subtree merge to bring in upstream changes
git merge -s recursive -X subtree=Modules/ThirdParty/DoubleConversion/src/double-conversion DoubleConversion-upstream
echo "---------------------------------"
echo NOW Fix all conflicts and test new code.
echo "---------------------------------"
echo Commit the merged/fixed/verified code and run this command
echo git rev-parse --short=8 DoubleConversion-upstream
echo "---------------------------------"
echo to get the commit hash from which the DoubleConversion-upstream
echo branch must be started on the next update.
echo "---------------------------------"
echo edit the line \"git branch DoubleConversion-upstream\" above.
echo Once you have commited this chage to the UpdateDoubleConversionFromGoogle.sh script,
echo use \"git gerrit-push\" to push this new update branch back to itk.org.
