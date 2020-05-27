void main(){
    int i;
    for(i = 0 ; i < 10 ; i++){
        do{
            while(i < 10){
                i = i + 1;
            }
            i = i - 1;
        }while(i > 1);
    }
    print("la variable i doit valoir 9 et vaut", i);
}