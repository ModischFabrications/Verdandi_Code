#!/usr/bin/env bash

echo "[MPC] --- deploying to target device ---"

echo "[MPC] prepare /data"
rm -r ./data/
mkdir ./data/

echo "[MPC] copying /website to /data"
cp -rf ./website/* ./data/

echo "[MPC] converting to *.gz"

gzip -rf ./data*

pio run --target uploadfs

