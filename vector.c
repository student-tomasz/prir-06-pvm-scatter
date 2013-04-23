#include "vector.h"

const int v_len = 20;
const double v[] = {36.94, 11.94, 9.21, -34.02, 24.38, 15.17, 27.96, 22.03, 21.08, -13.68, 29.72, 20.62, 28.23, -35.45, -12.83, -13.75, 7.47, -32.50, -8.29, -28.85};
const double v_sum = 75.38;

double sum_u(const double *u, const int u_len)
{
    double result = 0.0;
    int i;
    for (i = 0; i < u_len; i++) {
        result += u[i];
    }
    return result;
}
