#!/usr/bin/env bash

list_sources_file=./scripts/list.txt
dirs_to_check=$(cat ./scripts/dirs_to_check.txt)

for i in "${dirs_to_check[@]}";
do
    find $i -name "*.[hc]" >> $list_sources_file
done

echo "Checking"
echo "---"
cat $list_sources_file
echo "---"

echo ""
echo "Results"
echo "---"
uncrustify -c .uncrustify.cfg --replace --no-backup -F $list_sources_file > /dev/null
res=$?
rm -rf $list_sources_file

echo "==="
if [[ $res -eq "0" ]]; then
    echo "PASS"
else
    echo "FAIL"
fi

exit $res
