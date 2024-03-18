#!/bin/bash

cd /personal-projects/docker/nikola
#nikola init -q --demo daughtrey.com
cd daughtrey.com
nikola build
nikola serve
