#!/bin/bash
#URL="https://clarkesworldmagazine.com/monette_10_06"

do_pandoc()
{
    FMT=$1
    for ff in *.html; do
        title="${ff%.*}"
        if [[ -f "${title}.epub" ]]; then
            echo ${title} already exists, skipping
            continue
        fi
#        if [[ "${FMT}" == "mobi" ]]; then
#           cat "${title}.html" | sed 's/<p>/\r/g;s/<\/p>//g' > "/tmp/${title}"
#           pandoc --to epub --metadata title:"${title}" "/tmp/${title}" -o "${title}_${FMT}.${FMT}"
#       else
            pandoc --to epub --metadata title:"${title}" "${title}.html" -o "${title}_${FMT}.${FMT}"
#       fi
   done 
}

do_epub()
{
    for ff in *.html; do
        title="${ff%.*}"
        if [[ -f "${title}.epub" ]]; then
            echo ${title} already exists, skipping
            continue
        fi
        pandoc --to epub --metadata title:"${title}" "${title}.html" -o "${title}.epub"
    done 
#pandoc --to epub ${URL} -o test.epub
#pandoc --to epub ${URL} -o test.epub
#pandoc --to epub -o master_of_reality.epub ${URL}
}

do_rtf()
{
    for ff in *.html; do
        title="${ff%.*}"
        if [[ -f "${title}.rtf" ]]; then
            echo ${title} already exists, skipping
            continue
        fi
        pandoc --to rtf --metadata title:"${title}" "${title}.html" -o "${title}.rtf"
    done 
}

do_scrape()
{
    URL=https://clarkesworldmagazine.com/prior/?y=
    for (( YEAR=2007; YEAR<2022; YEAR++)); do
        ./scrape.py ${URL}${YEAR}
    done
}

case "$1" in 
    scrape) do_scrape ;; # ./scrape.py ${URL} ;;
    epub|rtf|mobi|azw3) do_pandoc $1 ;;
#    rtf) do_rtf ;;
    *) echo What? ;;
    esac
