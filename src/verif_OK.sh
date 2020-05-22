PATH="Tests/Verif/OK/"
cd $PATH
for i in *.c ; do
	echo Test du fichier "$i"
	../../.././minicc "$i" 
done
