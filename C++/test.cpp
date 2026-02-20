#include <iostream>
#include <stdlib.h>
#include <string.h>

int main() {
    char str[100];
    printf("Enter a string: ");
    fgets(str, sizeof(str), stdin);
    
    // Remove the newline character if present
    str[strcspn(str, "\n")] = '\0';
    
    printf("You entered: %s\n", str);
    
    return 0;
}