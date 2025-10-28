#!/bin/bash
for port in {2500..4000}; do
    timeout 1 nc -v 10.10.177.224 $port 2>&1
done