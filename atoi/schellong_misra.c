
/* Misra-konformer Code:       */


#define F_DIG(c) ((c) - '0' <= '9')

int d1[] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
int d2[] = { 0, 100, 200, 300, 400, 500, 600, 700, 800, 900 };
int d3[] = { 0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000 };
int d4[] = { 0, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000 };

int my_atoi(const char *s) {
    const unsigned char *a = (const unsigned char *)s;
    register int i;
    int m=0;
    if (*a<='0')  {
      if (a[0]==0||a[1]==0)  return (0);
      while (*a==' '||*a=='\t')  ++a;
      if (*a=='-'&&(m=1,1)||*a=='+')  ++a;
      while (*a=='0')  ++a;
    }
    i=0;

    if (F_DIG(a[0]))  {
      if (F_DIG(a[1]))  {
        if (F_DIG(a[2]))  {
          if (F_DIG(a[3]))  {
            if (F_DIG(a[4]))  {
              i+= d4[a[0]-'0'];
              i+= d3[a[1]-'0'];
              i+= d2[a[2]-'0'];
              i+= d1[a[3]-'0'];
              i+=    a[4]-'0';
            }
            else  {
              i+= d3[a[0]-'0'];
              i+= d2[a[1]-'0'];
              i+= d1[a[2]-'0'];
              i+=    a[3]-'0';
            }
          }
          else  {
            i+= d2[a[0]-'0'];
            i+= d1[a[1]-'0'];
            i+=    a[2]-'0';
          }
        }
        else  {
          i+= d1[a[0]-'0'];
          i+=    a[1]-'0';
        }
      }
      else  {
        i+=    a[0]-'0';
      }
    }
    else  {
      ;
    }
    if (m)  { i= -i; }
    else    { ; }
    return (i);
}
