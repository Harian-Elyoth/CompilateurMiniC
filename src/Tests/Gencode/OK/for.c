void main(){
    int i;
    int a = 0;
    int j;
    for(i = 5 ; i < 9 ; i = i + 1){
        j = i - 5;
        a = a + i;
    }
    print("A la fin du programme, et a vaut ", a, " et doit valoir 26");
}