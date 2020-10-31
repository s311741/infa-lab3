#!/usr/bin/bash
for ((i = 0; i < 1000; i++)); do
	./$1 < timetable.xml > /dev/null
done
