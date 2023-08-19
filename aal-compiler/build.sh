bash compile.sh
if [ $? -eq 1 ]
then
	echo "Compilation error"
	exit
fi
bin/aalc source.aal -o output -v

if [ $? -eq 1 ]
then
        echo "AALC Compilation error"
        exit
fi

./output
