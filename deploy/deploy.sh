#!/usr/bin/env bash

echo "[MPC] --- deploying to target device ---"

echo "[MPC] copying /website to /data"
cp -rf ./website/ ./data/

echo "[MPC] converting to *.gz"
for file in ./website*; do
  gzip $file
done

pio pio run --target uploadfs

