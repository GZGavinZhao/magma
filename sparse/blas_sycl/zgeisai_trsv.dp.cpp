/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date

       @precisions normal z -> c d s

*/
#include <sycl/sycl.hpp>
#include <dpct/dpct.hpp>
#include "magmasparse_internal.h"
#include "shuffle.dp.hpp"
#include <cmath>

#define PRECISION_z
#define COMPLEX
#define BLOCKSIZE 256
#define WARP_SIZE 32
#define WRP 32
#define WRQ 1


void ztrsv_lower_kernel_general(magmaDoubleComplex *dA, magmaDoubleComplex *dB, int *sizes,
                                sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);

    magmaDoubleComplex rB[ 2 ];
    magmaDoubleComplex rA[ 2 ];

    int n;
    int k;
    int N = sizes[j];

    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;


    // Read B to regs.
    #pragma unroll
    for (n = 0; n < 2; n++)
        rB[n] = dB[n*WARP_SIZE+idn];


    // Triangular solve in regs.
    #pragma unroll
    for (k = 0; k < N; k++)
    {
        #pragma unroll
        for (n = 0; n < 2; n++)
            rA[n] = dA[k*WARP_SIZE+n*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB[k/WARP_SIZE] /= rA[k/WARP_SIZE];

        magmaDoubleComplex top = magmablas_zshfl(rB[k/WARP_SIZE], k%WARP_SIZE, item_ct1);

        #pragma unroll
        for (n = 0; n < 2; n++)
            if (n*WARP_SIZE+idn > k)
                rB[n] -= (top*rA[n]);
    }
    // Drop B to dev mem.
    #pragma unroll
    for (n = 0; n < 2; n++)
        if (n*WARP_SIZE+idn < N)
            dB[n*WARP_SIZE+idn] = rB[n];

#endif
}



void ztrsv_upper_kernel_general(magmaDoubleComplex *dA, magmaDoubleComplex *dB, int *sizes,
                                sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);

    magmaDoubleComplex rB[ 2 ];
    magmaDoubleComplex rA[ 2 ];

    int n;
    int k;
    int N = sizes[j];

    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;


    // Read B to regs.
    #pragma unroll
    for (n = 0; n < 2; n++)
        rB[n] = dB[n*WARP_SIZE+idn];


    // Triangular solve in regs.
    #pragma unroll
    for (int k = N-1; k > -1; k--)
    {
        #pragma unroll
        for (n = 0; n < 2; n++)
            rA[n] = dA[k*WARP_SIZE+n*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB[k/WARP_SIZE] /= rA[k/WARP_SIZE];

        magmaDoubleComplex top = magmablas_zshfl(rB[k/WARP_SIZE], k%WARP_SIZE, item_ct1);

        #pragma unroll
        for (n = 0; n < 2; n++)
            if (n*WARP_SIZE+idn < k)
                rB[n] -= (top*rA[n]);
    }
    // Drop B to dev mem.
    #pragma unroll
    for (n = 0; n < 2; n++)
        if (n*WARP_SIZE+idn < N)
            dB[n*WARP_SIZE+idn] = rB[n];

#endif
}




void ztrsv_lower_kernel_1(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 1; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_2(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 2; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_3(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 3; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_4(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 4; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_5(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 5; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_6(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 6; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_7(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 7; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_8(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 8; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_9(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 9; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_10(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 10; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_11(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 11; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_12(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 12; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_13(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 13; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_14(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 14; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_15(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 15; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_16(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 16; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_17(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 17; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_18(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 18; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_19(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 19; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_20(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 20; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_21(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 21; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_22(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 22; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_23(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 23; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_24(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 24; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_25(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 25; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_26(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 26; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_27(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 27; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_28(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 28; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_29(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 29; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_30(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 30; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_31(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 31; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_lower_kernel_32(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 0; k < 32; k++)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex top = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn > k)
            rB -= (top*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}



void ztrsv_lower_kernel_switch(magmaDoubleComplex *dA, magmaDoubleComplex *dB, int *sizes, int num_rows ,
                               sycl::nd_item<3> item_ct1)
{
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    if (j < num_rows) {
        int N = sizes[j];
        switch( N ) {
            case  1:
                ztrsv_lower_kernel_1(dA, dB, item_ct1); break;
            case  2:
                ztrsv_lower_kernel_2(dA, dB, item_ct1); break;
            case  3:
                ztrsv_lower_kernel_3(dA, dB, item_ct1); break;
            case  4:
                ztrsv_lower_kernel_4(dA, dB, item_ct1); break;
            case  5:
                ztrsv_lower_kernel_5(dA, dB, item_ct1); break;
            case  6:
                ztrsv_lower_kernel_6(dA, dB, item_ct1); break;
            case  7:
                ztrsv_lower_kernel_7(dA, dB, item_ct1); break;
            case  8:
                ztrsv_lower_kernel_8(dA, dB, item_ct1); break;
            case  9:
                ztrsv_lower_kernel_9(dA, dB, item_ct1); break;
            case  10:
                ztrsv_lower_kernel_10(dA, dB, item_ct1); break;
            case  11:
                ztrsv_lower_kernel_11(dA, dB, item_ct1); break;
            case  12:
                ztrsv_lower_kernel_12(dA, dB, item_ct1); break;
            case  13:
                ztrsv_lower_kernel_13(dA, dB, item_ct1); break;
            case  14:
                ztrsv_lower_kernel_14(dA, dB, item_ct1); break;
            case  15:
                ztrsv_lower_kernel_15(dA, dB, item_ct1); break;
            case  16:
                ztrsv_lower_kernel_16(dA, dB, item_ct1); break;
            case  17:
                ztrsv_lower_kernel_17(dA, dB, item_ct1); break;
            case  18:
                ztrsv_lower_kernel_18(dA, dB, item_ct1); break;
            case  19:
                ztrsv_lower_kernel_19(dA, dB, item_ct1); break;
            case  20:
                ztrsv_lower_kernel_20(dA, dB, item_ct1); break;
            case  21:
                ztrsv_lower_kernel_21(dA, dB, item_ct1); break;
            case  22:
                ztrsv_lower_kernel_22(dA, dB, item_ct1); break;
            case  23:
                ztrsv_lower_kernel_23(dA, dB, item_ct1); break;
            case  24:
                ztrsv_lower_kernel_24(dA, dB, item_ct1); break;
            case  25:
                ztrsv_lower_kernel_25(dA, dB, item_ct1); break;
            case  26:
                ztrsv_lower_kernel_26(dA, dB, item_ct1); break;
            case  27:
                ztrsv_lower_kernel_27(dA, dB, item_ct1); break;
            case  28:
                ztrsv_lower_kernel_28(dA, dB, item_ct1); break;
            case  29:
                ztrsv_lower_kernel_29(dA, dB, item_ct1); break;
            case  30:
                ztrsv_lower_kernel_30(dA, dB, item_ct1); break;
            case  31:
                ztrsv_lower_kernel_31(dA, dB, item_ct1); break;
            case  32:
                ztrsv_lower_kernel_32(dA, dB, item_ct1); break;
            default:
                ztrsv_lower_kernel_general(dA, dB, sizes, item_ct1); break;
        }
    }
}

void ztrsv_upper_kernel_1(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 1-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_2(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 2-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_3(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 3-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_4(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 4-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_5(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 5-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_6(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 6-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_7(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 7-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_8(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 8-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_9(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                          sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 9-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_10(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 10-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_11(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 11-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_12(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 12-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_13(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 13-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_14(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 14-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_15(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 15-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_16(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 16-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_17(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 17-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_18(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 18-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_19(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 19-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_20(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 20-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_21(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 21-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_22(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 22-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_23(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 23-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_24(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 24-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_25(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 25-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_26(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 26-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_27(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 27-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_28(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 28-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_29(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 29-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_30(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 30-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_31(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 31-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}




void ztrsv_upper_kernel_32(magmaDoubleComplex *dA, magmaDoubleComplex *dB ,
                           sycl::nd_item<3> item_ct1)
{
#ifdef REAL
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    int idn = item_ct1.get_local_id(2);
    magmaDoubleComplex rB;
    magmaDoubleComplex rA;
    dA += (j)*WARP_SIZE*WARP_SIZE;
    dB += (j)*WARP_SIZE;

    // Read B to regs.
    rB = dB[idn];
    // Triangular solve in regs.
    #pragma unroll
    for (int k = 32-1; k >-1; k--)
    {
        rA = dA[k*WARP_SIZE+idn];
        if (k%WARP_SIZE == idn)
            rB /= rA;
        magmaDoubleComplex bottom = magmablas_zshfl(rB, k%WARP_SIZE, item_ct1);
        if ( idn < k)
            rB -= (bottom*rA);
    }
    // Drop B to dev mem.
    dB[idn] = rB;
#endif
}



void ztrsv_upper_kernel_switch(magmaDoubleComplex *dA, magmaDoubleComplex *dB, int *sizes, int num_rows ,
                               sycl::nd_item<3> item_ct1)
{
    int j = item_ct1.get_group(1) * item_ct1.get_group_range(2) +
            item_ct1.get_group(2);
    if (j < num_rows) {
        int N = sizes[j];
        switch( N ) {
            case  1:
                ztrsv_upper_kernel_1(dA, dB, item_ct1); break;
            case  2:
                ztrsv_upper_kernel_2(dA, dB, item_ct1); break;
            case  3:
                ztrsv_upper_kernel_3(dA, dB, item_ct1); break;
            case  4:
                ztrsv_upper_kernel_4(dA, dB, item_ct1); break;
            case  5:
                ztrsv_upper_kernel_5(dA, dB, item_ct1); break;
            case  6:
                ztrsv_upper_kernel_6(dA, dB, item_ct1); break;
            case  7:
                ztrsv_upper_kernel_7(dA, dB, item_ct1); break;
            case  8:
                ztrsv_upper_kernel_8(dA, dB, item_ct1); break;
            case  9:
                ztrsv_upper_kernel_9(dA, dB, item_ct1); break;
            case  10:
                ztrsv_upper_kernel_10(dA, dB, item_ct1); break;
            case  11:
                ztrsv_upper_kernel_11(dA, dB, item_ct1); break;
            case  12:
                ztrsv_upper_kernel_12(dA, dB, item_ct1); break;
            case  13:
                ztrsv_upper_kernel_13(dA, dB, item_ct1); break;
            case  14:
                ztrsv_upper_kernel_14(dA, dB, item_ct1); break;
            case  15:
                ztrsv_upper_kernel_15(dA, dB, item_ct1); break;
            case  16:
                ztrsv_upper_kernel_16(dA, dB, item_ct1); break;
            case  17:
                ztrsv_upper_kernel_17(dA, dB, item_ct1); break;
            case  18:
                ztrsv_upper_kernel_18(dA, dB, item_ct1); break;
            case  19:
                ztrsv_upper_kernel_19(dA, dB, item_ct1); break;
            case  20:
                ztrsv_upper_kernel_20(dA, dB, item_ct1); break;
            case  21:
                ztrsv_upper_kernel_21(dA, dB, item_ct1); break;
            case  22:
                ztrsv_upper_kernel_22(dA, dB, item_ct1); break;
            case  23:
                ztrsv_upper_kernel_23(dA, dB, item_ct1); break;
            case  24:
                ztrsv_upper_kernel_24(dA, dB, item_ct1); break;
            case  25:
                ztrsv_upper_kernel_25(dA, dB, item_ct1); break;
            case  26:
                ztrsv_upper_kernel_26(dA, dB, item_ct1); break;
            case  27:
                ztrsv_upper_kernel_27(dA, dB, item_ct1); break;
            case  28:
                ztrsv_upper_kernel_28(dA, dB, item_ct1); break;
            case  29:
                ztrsv_upper_kernel_29(dA, dB, item_ct1); break;
            case  30:
                ztrsv_upper_kernel_30(dA, dB, item_ct1); break;
            case  31:
                ztrsv_upper_kernel_31(dA, dB, item_ct1); break;
            case  32:
                ztrsv_upper_kernel_32(dA, dB, item_ct1); break;
            default:
                ztrsv_upper_kernel_general(dA, dB, sizes, item_ct1); break;
        }
    }
}
/**
    Purpose
    -------
    Does all triangular solves

    Arguments
    ---------


    @param[in]
    uplotype    magma_uplo_t
                lower or upper triangular

    @param[in]
    transtype   magma_trans_t
                possibility for transposed matrix

    @param[in]
    diagtype    magma_diag_t
                unit diagonal or not

    @param[in]
    L           magma_z_matrix
                Matrix in CSR format

    @param[in]
    LC          magma_z_matrix
                same matrix, also CSR, but col-major

    @param[out]
    sizes       magma_int_t*
                Number of Elements that are replaced.

    @param[out]
    locations   magma_int_t*
                Array indicating the locations.

    @param[out]
    trisystems  magmaDoubleComplex*
                trisystems

    @param[out]
    rhs         magmaDoubleComplex*
                right-hand sides

    @param[in]
    queue       magma_queue_t
                Queue to execute in.

    @ingroup magmasparse_zaux
    ********************************************************************/

extern "C" magma_int_t
magma_zmtrisolve_batched_gpu(
    magma_uplo_t uplotype,
    magma_trans_t transtype,
    magma_diag_t diagtype,
    magma_z_matrix L,
    magma_z_matrix LC,
    magma_index_t *sizes,
    magma_index_t *locations,
    magmaDoubleComplex *trisystems,
    magmaDoubleComplex *rhs,
    magma_queue_t queue )
{
    magma_int_t info = 0;

    int blocksize1 = WARP_SIZE;
    int blocksize2 = 1;
    int dimgrid1 = min( int( sqrt( double( LC.num_rows ))), 65535 );
    int dimgrid2 = min(magma_ceildiv( LC.num_rows, dimgrid1 ), 65535);
    int dimgrid3 = magma_ceildiv( LC.num_rows, dimgrid1*dimgrid2 );

    sycl::range<3> grid(dimgrid3, dimgrid2, dimgrid1);
    sycl::range<3> block(1, blocksize2, blocksize1);

    if( uplotype == MagmaLower ){
        ((sycl::queue *)(queue->sycl_stream()))
            ->parallel_for(sycl::nd_range<3>(grid * block, block),
                           [=](sycl::nd_item<3> item_ct1) {
                               ztrsv_lower_kernel_switch(trisystems, rhs, sizes,
                                                         LC.num_rows, item_ct1);
                           });
    } else {
        ((sycl::queue *)(queue->sycl_stream()))
            ->parallel_for(sycl::nd_range<3>(grid * block, block),
                           [=](sycl::nd_item<3> item_ct1) {
                               ztrsv_upper_kernel_switch(trisystems, rhs, sizes,
                                                         LC.num_rows, item_ct1);
                           });
    }

    return info;
}
