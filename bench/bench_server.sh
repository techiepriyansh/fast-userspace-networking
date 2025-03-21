#!/bin/bash

set -o errexit

source "$(dirname "$0")/../testenv/config.sh"
source $STATEDIR/veth-adv03.state

iperf3 -s -B $OUTSIDE_IP4 -i 1

