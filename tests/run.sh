#!/bin/sh
# Builds and runs every test with sanitizers: AddressSanitizer and
# UndefinedBehaviorSanitizer for all of them, ThreadSanitizer for the threaded
# ones. Usage: tests/run.sh [compiler], default c++
set -e
CXX=${1:-c++}
cd "$(dirname "$0")"
out=$(mktemp -d)
trap 'rm -rf "$out"' EXIT
flags="-std=c++20 -O1 -g -Wall -Wextra -pthread"

for t in vector bst avl redblack hashtable smartptr os; do
  $CXX $flags -fsanitize=address,undefined -fno-sanitize-recover=all \
    "${t}_test.cpp" -o "$out/$t"
  "$out/$t"
done
for t in smartptr os; do
  $CXX $flags -fsanitize=thread "${t}_test.cpp" -o "$out/$t-tsan"
  TSAN_OPTIONS="suppressions=$PWD/tsan.supp" "$out/$t-tsan"
done
echo "all tests passed ($CXX)"
