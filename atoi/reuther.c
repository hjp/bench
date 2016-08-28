#define F_DIG(c) ((c) - '0' <= '9')

int my_atoi(const char *s) {
  const unsigned char *p = (const unsigned char *)s;
  int m = 0;
  if (p[0] == '-') {
    m = 1;
    p++;
  }
  int idx = 0;
  int i = 0;
  while ((p[idx] != '\0') && F_DIG(p[idx])) {
     i = 10*i + (p[idx] - '0');
     ++idx;
  }
  return (m ? -i : i);
}
