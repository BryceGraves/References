#include <stdio.h> // This is here so I can use printf().
#include <string.h>

void display_results(int a, int b);

int
main(
    int argc        // The number of command line arguments.
    , char *argv[]  // An array of strings of the command line arguments.
)
{
    int i = 0;
    int j = 0;
    char *ch_ptr = NULL;
    char *ch_stuff = NULL;

    for(i = 0, j = 10; i < 5; i++, j--) {
        display_results(j, i);
    }

    ch_stuff = "Jesse";
    strcpy(ch_ptr, "hello world");
    printf("%s\n", ch_ptr);

    ch_stuff[0] = 'M';

    return(0);
}

void display_results(int a, int b)
{
    printf("%d %d\n", a, b);
    printf("%d\n", a / (b % 3));
}
