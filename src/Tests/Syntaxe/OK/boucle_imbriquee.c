void main(){
    int i;
    for(i = 0 ; i < 5 ; i = i + 1){
        while(i < 2){
            int j = 5;
            i = j;
            j = j - 1;
        }
    }
}