#!/bin/bash
SITE=daughtrey.com
if [[ ! -d /home/mattd/sites/${SITE} ]]; then cd /home/mattd/sites; jekyll new ${SITE}; fi
cd /home/mattd/sites/${SITE}
bundle exec jekyll serve --host=0.0.0.0 --livereload
