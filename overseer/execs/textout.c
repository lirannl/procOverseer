#include <stdio.h>

int main(int args, char *argv[]){
    
        printf("this is a simple calculater program\n");
        int num1=0;
        printf("please enter the first number\n");
        scanf("%i", &num1);
        int num2=0;
        printf("please enter the second number\n");
        scanf("%i", &num2);
        int ans = num1 +num2;
        printf("the answer is %i\n", ans);  
    return 1;

}