#!/usr/bin/env bash

echo "[MPC] --- deploying to target device file system --- "

command -v pio >/dev/null 2>&1 || { echo >&2 "pio can't be found. Add to PATH!"; exit 1; }

echo "[MPC] prepare /data"
rm -r ./data/
mkdir ./data/

echo "[MPC] copying /website to /data"
cp -rf ./website/* ./data/

echo "[MPC] converting to *.gz"

gzip -rf ./data*

pio run --target uploadfs

if [ $? -eq 0 ]
then
  echo "[MPC] --- Finished successfully, don't forget to upload your program --- "
fi


