
#include <stdlib.h>
#include  <stdio.h>
#include <math.h>




void mgc2sp(double *mgc, const int m, const double a, const double g, double *x,
            double *y, const int flng);

void mgc2mgc(double *c1, const int m1, const double a1, const double g1,
             double *c2, const int m2, const double a2, const double g2);


void c2sp(double *c, const int m, double *x, double *y, const int l);


double *dgetmem(const int leng);


void movem(void *a, void *b, const size_t size, const int nitem);



char *getmem(const size_t leng, const size_t size);

void gnorm(double *c1, double *c2, int m, const double g);

void ignorm(double *c1, double *c2, int m, const double g);


void gc2gc(double *c1, const int m1, const double g1, double *c2, const int m2,
           const double g2);


int fftr(double *x, double *y, const int m);

static int checkm(const int m);


int fft(double *x, double *y, const int m);

void fillz(void *ptr, const size_t size, const int nitem);


void freqt(double *c1, const int m1, double *c2, const int m2, const double a);
