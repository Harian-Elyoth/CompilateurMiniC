PATH="Tests/Syntaxe/KO/"
cd $PATH
for i in *.c ; do
	echo Test du fichier "$i"
	../../.././minicc "$i" -s
done
