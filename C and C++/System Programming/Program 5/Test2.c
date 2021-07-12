char a1[4];
char *ptrc;
int i1;
float f1;
double d1;
struct TS
{ char c[8];
  int j;
  char *ptrc;
}s;
int i2;
float f2;
double d2;
char a2[4];


int main(void) {
  printf("%d\n", s.c - &a2[1]);
  return 0;
}
