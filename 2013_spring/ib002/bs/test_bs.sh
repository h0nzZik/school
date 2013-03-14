#!/bin/sh

echo 'Printing error statistics'
echo "Error log from '`date`':" > error

for x in `seq 0 10000`; do
	./bs > .test_output
	if [ "$?" == "0" ]; then
		echo -n .
	else
		echo "Error for test $x" >> error
		cat .test_output >> error
		echo "End." >> error
		echo -n x
	fi
done

echo 'done'
