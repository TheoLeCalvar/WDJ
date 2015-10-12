#include "julia.h"

int iterateOverJulia(double r, double i, double cr, double ci, int iterations) {
    // float x = (z.x * z.x - z.y * z.y) + c.x;
    // float y = (z.x * z.y + z.x * z.y) + c.y;
    //
    // if ((x * x + y * y) > 10.0) break;
    // z.x = x;
    // z.y = y;
    double r_, i_, r2, i2, ri, ir2;

    r_ = r;
    i_ = i;

    for (int j = 0; j < iterations; ++j) {
        r2 = r_ * r_;
        i2 = i_ * i_;
        ri = r_ * i_;
        ir2 = r2 + i2;

        if (ir2 - 10 > 0) {
            return j;
        }

        ri = ri * 2;
        i_ = ri + ci;

        r2 -= i2;
        r_ = r2 + cr;
    }

    return -1;
}
