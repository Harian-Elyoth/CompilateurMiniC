PATH="Tests/Syntaxe/OK/"
cd $PATH
for i in *.c ; do
	echo Test du fichier "$i"
	../../.././minicc "$i" -s
done
