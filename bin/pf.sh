#!/bin/bash
echo "rdr pass inet proto tcp from any to any port 2222 -> raspberrypi.local port 22" | sudo pfctl -ef -
