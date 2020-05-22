PATH="Tests/Verif/KO/"
cd $PATH
for i in *.c ; do
	echo Test du fichier "$i"
	../../.././minicc "$i" 
done
