#include <errno.h>
#include "fake.h"

int main(){
    FILE *fake_reader = fopen("Fakefile","r");
    if (fake_reader == NULL) 
    { 
        printf("Could not open the Fakefile!!! \n"); 
        return 0; 
    }

    

    fclose(fake_reader); 
}