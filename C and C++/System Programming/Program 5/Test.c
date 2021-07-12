struct tS1
{ int a[4];
};

struct tElement
{ int *pInt[2];
  struct tS1 S1;
  int m, n;
};

int i, j;
int *pi[2];
struct tElement E[2] ;
struct tElement *pE[2];
int k[2];
struct tElement N;

int main(void) {

  printf("%d\n",&E[ 0 ].n - &i);
  return 0;
}
