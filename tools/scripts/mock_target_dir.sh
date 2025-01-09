#!/bin/bash

TARGET_DIR="./target"
TARGET_DIR_DEBUG="./target/debug"
TARGET_DIR_RELEASE="./target/release"

create_files_in_target_dir() {
    mkdir -p $1
    for i in {0..1000}; do
        touch $1/file_$i
    done
}

create_files_in_target_dir $TARGET_DIR_DEBUG
create_files_in_target_dir $TARGET_DIR_RELEASE
