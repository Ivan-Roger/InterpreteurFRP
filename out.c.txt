// Code generated from FRP Compiler (by Ivan ROGER)
int main () {
    // Variables :
    int a;
    int b;
    int i;
    int j;
    int x;
    int y;

    // Code :
    a = (5);
    printf("Saisir a et b : ");
    scanf("%d%d", a, b);
    i = (12+3);
    while (i>7) {
        i = (i-1);
    }//;
    do {
        i = (i+1);
    } while (i>9);
    if (i>10) {
        j = (1);
    } else if (i>7) {
        j = (2);
    } else {
        j = (9);
    }//;
    for (x = (0); x<10; x = (x+1)) {
        y = (x*2);
        printf("\ty=%d\n", y);
    }//;
    printf("Bonjour, %d%d", 5, a);
}
// Done in 3ms.
