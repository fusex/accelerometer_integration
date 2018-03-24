#!/usr/bin/env bash

script_dir=$(dirname "$0")
data_dir=$(find . -maxdepth 1 -mindepth 1 -type d -printf '%f\n')
while read -r dir; do
    if [ ! -f "$dir/merged_data" ]; then
	find "$dir" -name "data_*" | sort -V | xargs cat > "$dir/merged_data"
    fi
done <<< "$data_dir"
