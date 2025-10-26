#!/bin/bash
for port in {1..100}; do
    timeout 1 nc -v 10.10.24.24 $port 2>&1 | tail -n 1
done