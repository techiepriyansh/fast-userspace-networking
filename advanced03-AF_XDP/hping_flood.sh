#!/bin/bash

# Save current ICMP rate limit values
DEFAULT_RATELIMIT=$(sysctl -n net.ipv4.icmp_ratelimit)
DEFAULT_RATEMASK=$(sysctl -n net.ipv4.icmp_ratemask)

cleanup() {
    echo "Restoring default ICMP rate limits..."
    sudo sysctl -w net.ipv4.icmp_ratelimit=$DEFAULT_RATELIMIT
    sudo sysctl -w net.ipv4.icmp_ratemask=$DEFAULT_RATEMASK
    exit 0
}

trap cleanup SIGINT SIGTERM

if ! sudo sysctl -w net.ipv4.icmp_ratelimit=0; then
    echo "Permission denied: Failed to modify icmp_ratelimit."
    cleanup
fi

if ! sudo sysctl -w net.ipv4.icmp_ratemask=0; then
    echo "Permission denied: Failed to modify icmp_ratemask."
    cleanup
fi

source "$(dirname "$0")/../testenv/config.sh"
source $STATEDIR/veth-adv03.state

if ! "$(dirname "$0")/../testenv/testenv.sh" exec -- hping3 --icmp --flood -I veth0 $OUTSIDE_IP4; then
    cleanup
fi

cleanup