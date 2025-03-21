#!/bin/bash

set -o errexit

source "$(dirname "$0")/../testenv/config.sh"
source $STATEDIR/veth-adv03.state

"$(dirname "$0")/../testenv/testenv.sh" exec -- iperf3 -c $OUTSIDE_IP4 -u -b 0 -t 10 -i 1 -B $INSIDE_IP4
