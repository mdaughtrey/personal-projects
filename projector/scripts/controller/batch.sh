#!/bin/bash

#titles="mt_42 mt_42_42 mt_42_42_43 mt_42_43 mt_43 mt_43_43 mt_42_43_43"
titles="42_43_43_43 42_43_43_43_43 42_43_43_43_43_43 42_43_43_43_43_43_43 42_43_43_43_43_43_43_43 42_43_43_43_43_43_43_43_43 42_43_43_43_43_43_43_43_43_43 42_43_43_43_43_43_43_43_43_43_43 42_43_43_43_43_43_43_43_43_43_43_43 42_43_43_43_43_43_43_43_43_43_43_43_43"

titles=1_of_9
for t in ${titles}; do
	PROJECT=${t} ./runhq.sh import
	PROJECT=${t} ./runhq.sh pngcrop
done
