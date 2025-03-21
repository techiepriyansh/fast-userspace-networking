#!/bin/bash

set -o errexit

"$(dirname "$0")/../testenv/testenv.sh" setup --name veth-adv03 --legacy-ip

