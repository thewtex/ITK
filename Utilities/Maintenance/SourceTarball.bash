#!/usr/bin/env bash
#==========================================================================
#
#   Copyright Insight Software Consortium
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0.txt
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#==========================================================================*/

usage() {
  die "USAGE: SourceTarball.bash [--tgz|--zip] [-v <version>] [<tag>|<commit>]"
}

info() {
  echo "$@" 1>&2
}

die() {
  echo "$@" 1>&2; exit 1
}

return_pipe_status() {
  echo ${PIPESTATUS[@]} |grep -q -v "[1-9]"
}

find_data_objects() {
  git ls-tree --full-tree -r "$1" |
  egrep '\.(md5)$' |
  while read mode type obj path; do
    case "$path" in
      *.md5)  echo MD5/$(git cat-file blob $obj) ;;
      *)      die "Unknown ExternalData content link: $path" ;;
    esac
  done | sort | uniq
  return_pipe_status
}

validate_MD5() {
  md5sum=$(md5sum "$1" | sed 's/ .*//') &&
  if test "$md5sum" != "$2"; then
    die "Object MD5/$2 is corrupt: $1"
  fi
}

download_object() {
  algo="$1" ; hash="$2" ; path="$3"
  mkdir -p $(dirname "$path") &&
  if wget "http://www.itk.org/files/ExternalData/$algo/$hash" -O "$path.tmp$$" 1>&2; then
    mv "$path.tmp$$" "$path"
  else
    rm -f "$path.tmp$$"
    false
  fi
}

index_data_objects() {
  # Input lines have format <algo>/<hash>
  while IFS=/ read algo hash; do
    # Final path in source tarball
    path=".ExternalData/$algo/$hash"
    # Find the object file on disk
    if test -f "$path"; then
      file="$path" # available in place
    elif test -f ~/"$path" ; then
      file=~/"$path" # available in home dir
    else
      download_object "$algo" "$hash" "$path" &&
      file="$path"
    fi &&
    validate_$algo "$file" "$hash" &&
    obj=$(git hash-object -t blob -w "$file") &&
    echo "100644 blob $obj	$path" ||
    return
  done |
  git update-index --index-info
  return_pipe_status
}

load_data_objects() {
  find_data_objects "$1" |
  index_data_objects
  return_pipe_status
}

git_archive_tgz() {
  git -c core.autocrlf=false archive --format=tar --prefix=$out/ $1 | gzip > $2.tar.gz &&
  info "Wrote $2.tar.gz"
}

git_archive_zip() {
  git -c core.autocrlf=true archive --format=zip --prefix=$out/ $1 > $2.zip &&
  info "Wrote $2.zip"
}

#-----------------------------------------------------------------------------

format=tgz
commit=
version=

# Parse command line options.
while test $# != 0; do
  case "$1" in
    --tgz) format=tgz ;;
    --zip) format=zip ;;
    --) shift; break ;;
    -v) shift; version="$1" ;;
    -*) usage ;;
    *) test -z "$commit" && commit="$1" || usage ;;
  esac
  shift
done
test $# = 0 || usage
test -n "$commit" || commit=HEAD

if ! git rev-parse --verify -q "$commit" >/dev/null ; then
  die "'$commit' is not a valid commit"
fi
if test -z "$version"; then
  desc=$(git describe $commit) &&
  if test "${desc:0:1}" != "v"; then
    die "'git describe $commit' is '$desc'; use -v <version>"
  fi &&
  version=${desc#v} &&
  echo "$commit is version $version"
fi

# Create temporary git index to construct source tree
export GIT_INDEX_FILE="$(pwd)/tmp-$$-index" &&
trap "rm -rf '$GIT_INDEX_FILE'" EXIT &&

info "Loading tree from $commit..." &&
git read-tree -m -i $commit &&

info "Loading data for $commit..." &&
load_data_objects $commit &&

info "Generating source archive..." &&
tree=$(git write-tree) &&
git_archive_$format $tree "InsightToolkit-$version"
