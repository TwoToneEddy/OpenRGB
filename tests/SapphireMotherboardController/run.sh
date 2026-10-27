#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
test_binary=$(mktemp /tmp/openrgb-sapphire-test.XXXXXX)
trap 'rm -f "$test_binary"' EXIT
${CXX:-c++} -std=c++17 -pthread -Wall -Wextra \
    -I. -Ii2c_smbus -Idependencies/json -IControllers/SapphireMotherboardController \
    tests/SapphireMotherboardController/test.cpp \
    Controllers/SapphireMotherboardController/SapphireMotherboardController.cpp \
    i2c_smbus/i2c_smbus.cpp -o "$test_binary"
"$test_binary"
echo 'Sapphire motherboard transport tests passed'
