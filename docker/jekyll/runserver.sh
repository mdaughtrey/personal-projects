#!/bin/bash
SITE=daughtrey.com
export GEM_HOME=/home/mattd/gems
export PATH=/home/mattd/gems/bin:${PATH}
if [[ ! -d /home/mattd/sites/${SITE} ]]; then 
    cd /home/mattd/sites
    jekyll new ${SITE}
fi
cd /home/mattd/sites/${SITE}
#bundle exec jekyll serve --host=0.0.0.0 --livereload
jekyll serve --host=0.0.0.0 --livereload
