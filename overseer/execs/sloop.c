int main(int args, char *argv[]){
    if(args >1){
        for(int i =1; i<args; i++){
        printf("%s \n", argv[i]);
        }  
    }
    else{
        printf("you done messed up");
    }
    return 1;
}