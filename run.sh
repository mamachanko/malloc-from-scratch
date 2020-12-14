#!/usr/bin/env bash

set -euo pipefail

gcc malloc.c -o malloc && ./malloc
