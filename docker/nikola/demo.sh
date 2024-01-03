#!/bin/bash

cd /home/mattd
nikola init -q --demo daughtrey.com
cd daughtrey.com
nikola build
nikola serve
