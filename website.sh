#!/bin/bash
set -eo pipefail

# build the doxide website in a reproducible way

# build doxide with code coverage enabled
mkdir build
cd build
CXXFLAGS="--coverage -O0 -fno-inline -fno-elide-constructors -Wall" \
  cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --parallel 16
cmake --install . --prefix $HOME/.local
cd ..

# run a few things to collect code coverage data
doxide build
mkdir hello
cd hello
doxide init
doxide cover > coverage.json
cd ..
rm -rf hello

# collate code coverage data
find . -name '*.gcda' | xargs gcov --stdout > coverage.gcov

# rebuild docs with code coverage report included
doxide build --coverage coverage.gcov
