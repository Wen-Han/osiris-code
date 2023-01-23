/*****************************************************************************************

Charge conserving current deposition, AVX-512 optimized version (single precision)

*****************************************************************************************/


/* if __TEMPLATE__ is defined then read the template definition at the end of the file */
#ifndef __TEMPLATE__

#include "os-spec-current-avx512.h"

#include "vector-avx512.h"
#include "splines-avx512.h"
#include "os-spec-push-avx512.h"

/*****************************************************************************************
vwl_s1
*****************************************************************************************/

inline void vwl_s1( __m512 const vqn, __m512 const vx0, __m512 const vx1, __m512 vwl[] )
{

  vwl[0] = _mm512_mul_ps(vqn, _mm512_sub_ps(vx1, vx0));
}


/*****************************************************************************************
vwl_s2
*****************************************************************************************/

inline void vwl_s2( __m512 const vqn, __m512 const vx0, __m512 const vx1, __m512 vwl[] )
{

  const __m512 c1_2 = _mm512_set1_ps( 0.5f );

  __m512 d    = _mm512_sub_ps( vx1, vx0 );
  __m512 s1_2 = _mm512_sub_ps( c1_2, vx0 );
  __m512 p1_2 = _mm512_add_ps( c1_2, vx0 );

  __m512 n    = _mm512_mul_ps( vqn, d );

  vwl[0] = _mm512_mul_ps( n, _mm512_fnmadd_ps( c1_2, d, s1_2 ));
  vwl[1] = _mm512_mul_ps( n, _mm512_fmadd_ps(  c1_2, d, p1_2 ));

}


/*****************************************************************************************
vwl_s3
*****************************************************************************************/

inline void vwl_s3( __m512 const vqn, __m512 const vx0, __m512 const vx1, __m512 vwl[] )
{
  __m512 const c1_2 = _mm512_set1_ps( 0.5f );
  __m512 const c3_4 = _mm512_set1_ps( 0.75f );
  __m512 const c1_3 = _mm512_set1_ps( 1.0f/3.0f );

  __m512   d = _mm512_sub_ps(  vx1, vx0 );
  __m512   s = _mm512_sub_ps( c1_2, vx0 );
  __m512   p = _mm512_add_ps( c1_2, vx0 );
  __m512 d_3 = _mm512_mul_ps( c1_3,   d );

  vwl[0] = _mm512_mul_ps( c1_2, _mm512_fmsub_ps( s, s, _mm512_mul_ps( d, _mm512_sub_ps( s, d_3 )) ) );
  vwl[1] = _mm512_fnmadd_ps( d, _mm512_add_ps( vx0, d_3 ), _mm512_fnmadd_ps( vx0, vx0, c3_4 ));
  vwl[2] = _mm512_mul_ps( c1_2, _mm512_fmadd_ps( p, p, _mm512_mul_ps( d, _mm512_add_ps( p, d_3 )) ) );

  __m512 n = _mm512_mul_ps( vqn, d );
  vwl[0] = _mm512_mul_ps( n, vwl[0] );
  vwl[1] = _mm512_mul_ps( n, vwl[1] );
  vwl[2] = _mm512_mul_ps( n, vwl[2] );
}

/*****************************************************************************************
vwl_s4
*****************************************************************************************/

inline void vwl_s4( __m512 const vqn, __m512 const vx0, __m512 const vx1, __m512 vwl[] ) {

  __m512 const c1_2 = _mm512_set1_ps( 0.5f );
  __m512 const c1_4 = _mm512_set1_ps( 0.25f );
  __m512 const c1_6 = _mm512_set1_ps( 1.0f/6.0f );
  __m512 const c3_2 = _mm512_set1_ps( 1.5f );
  __m512 const c3_4 = _mm512_set1_ps( 0.75f );
  __m512 const c1_3 = _mm512_set1_ps( 1.0f/3.0f );
  __m512 const c2_3 = _mm512_set1_ps( 2.0f/3.0f );

  __m512 d = _mm512_sub_ps( vx1, vx0 );
  __m512 s = _mm512_sub_ps( c1_2, vx0 );
  __m512 p = _mm512_add_ps( c1_2, vx0 );

  __m512 t = _mm512_sub_ps( vx0, c1_6 );
  __m512 u = _mm512_add_ps( vx0, c1_6 );

  __m512 s2 = _mm512_mul_ps( s, s );
  __m512 s3 = _mm512_mul_ps( s2, s );

  __m512 p2 = _mm512_mul_ps( p, p );
  __m512 p3 = _mm512_mul_ps( p2, p );

  __m512 d_2 = _mm512_mul_ps( d, c1_2 );
  __m512 d_4 = _mm512_mul_ps( d, c1_4 );

  vwl[0] = _mm512_mul_ps( c1_6, _mm512_fmadd_ps( d, _mm512_fmsub_ps( d, _mm512_sub_ps( s, d_4 ), _mm512_mul_ps( c3_2, s2 )  ), s3 ) );
  vwl[1] = _mm512_fmadd_ps( d, _mm512_fmadd_ps(d_2, _mm512_add_ps(t, d_4), _mm512_fmsub_ps(c3_4, _mm512_mul_ps(t, t), c1_3)),
                          _mm512_fmadd_ps(c1_2, p3, _mm512_sub_ps(c2_3, p2)));
  vwl[2] = _mm512_fnmadd_ps(d, _mm512_fmadd_ps(d_2, _mm512_add_ps(u, d_4), _mm512_fmsub_ps(c3_4, _mm512_mul_ps(u, u), c1_3)),
                          _mm512_fmadd_ps(c1_2, s3, _mm512_sub_ps(c2_3, s2)));
  vwl[3] = _mm512_mul_ps( c1_6, _mm512_fmadd_ps( d, _mm512_fmadd_ps( d, _mm512_add_ps( p, d_4 ), _mm512_mul_ps( c3_2, p2 )  ), p3 ) );

  __m512 n = _mm512_mul_ps( vqn, d );
  vwl[0] = _mm512_mul_ps( n, vwl[0] );
  vwl[1] = _mm512_mul_ps( n, vwl[1] );
  vwl[2] = _mm512_mul_ps( n, vwl[2] );
  vwl[3] = _mm512_mul_ps( n, vwl[3] );

}



/****************************************************************************************

  Generate specific current deposition functions for 2D and 3D, 1st to 4th order

****************************************************************************************/

#define __TEMPLATE__

// These macros will append _s1, _s2, etc to function names.
#define ONAME(f, o) OJOIN(f, o)
#define OJOIN(f, o) f ## _s ## o

/********************************** Linear interpolation ********************************/

// Interpolation order
#define ORDER 1
// Number of interpolation points ( ORDER + 1 )
#define NP 2
// Current grid offset (1 + ORDER/2)
#define OFFSET 1

#include __FILE__

/******************************** Quadratic interpolation *******************************/

#define ORDER 2
#define NP 3
#define OFFSET 2

#include __FILE__

/********************************** Cubic interpolation *********************************/

#define ORDER 3
#define NP 4
#define OFFSET 2

#include __FILE__

/********************************* Quartic interpolation ********************************/

#define ORDER 4
#define NP 5
#define OFFSET 3

#include __FILE__

#else

/****************************************************************************************

  Template function definitions for 2D and 3D current deposition

****************************************************************************************/


/*

Note: In 2D there was a division by 2 removed from the calculation of the perpendicular
      weights, so jnorm must be dx / dt / (2*NORM). In 3D it was a division by 3, so
      jnorm must be dx / dt / (3*NORM). To avoid repeating this every time the
      DEP_CURRENT functions are called, this is done in the ADVANCE_DEPOSIT functions.

*/

/***************   Generate Function names based on interpolation order  ****************/

#define DEP_CURRENT_1D ONAME( vdepcurrent_1d, ORDER )
#define DEP_CURRENT_2D ONAME( vdepcurrent_2d, ORDER )
#define DEP_CURRENT_3D ONAME( vdepcurrent_3d, ORDER )

#define SPLINE  ONAME( vspline, ORDER )
#define WL      ONAME( vwl, ORDER )

/****************************************************************************************
  1D current deposition
*****************************************************************************************/

/*
  Reference implementation, serial deposition
*/

#if 0

void DEP_CURRENT_1D
(float * const current, int const * const size, int const * const offset,
 float * const norm, t_split_buf1D * const part)
{

  typedef struct Current { float j1, j2, j3; } t_current;
  t_current *p0;

  fvec j2[NP],j3[NP];
  fvec vwl1[ORDER];

  __m512 vx0, vx1, vq, vvy, vvz;

  DECLARE_ALIGNED_64( int idx[VEC_WIDTH] );

  __m512 vqnx, vqvy, vqvz;
  __m512 vs0x[NP], vs1x[NP];

  int i, np;

  __m512 const c1_2  = _mm512_set1_ps( 0.5f );

  t_current* const pj = (t_current *) (current + ( offset[0] - OFFSET ) * 3 );

  __m512 const vnorm1 = _mm512_set1_ps(norm[0]);

  np = part -> np;

  // If number of particles in buffer is not multiple of VEC_WIDTH add dummy particles to the end

  if ( np % VEC_WIDTH ) {
    for( i = 0; i < VEC_WIDTH - np%VEC_WIDTH; i ++ ) {
       int k = np + i;

       part->x0[k] = part->x1[k] = 0.0f;
       part-> q[k] = part->vy[k] = part->vz[k] = 0.0f;

       part->ix[k] = 1;
    }
  }

  for( i = 0; i < np; i += VEC_WIDTH ) {

    int k, k1;
    __m512i vix;

    // load 16 particles
    LOAD16P1D( part, i, vx0, vx1, vq, vvy, vvz, vix )

    // Store cell index
    _mm512_store_epi32( (__m512i *) idx, vix );

    // Get splines
    SPLINE( vx0, vs0x );
    SPLINE( vx1, vs1x );

    vqnx = _mm512_mul_ps( vq, vnorm1 );
    vqvy = _mm512_mul_ps( vq, vvy );
    vqvz = _mm512_mul_ps( vq, vvz );

    // get longitudinal weights
    WL( vqnx, vx0, vx1, (__m512 *) vwl1 );

    // get j2,j3 current
    for ( k1 = 0; k1 < NP; k1++ ) {
      __m512 vwp1 = _mm512_mul_ps( c1_2, _mm512_add_ps(vs0x[k1], vs1x[k1]) );
      j2[k1].v16  = _mm512_mul_ps( vqvy, vwp1 );
      j3[k1].v16  = _mm512_mul_ps( vqvz, vwp1 );
    }

    // Loop by particle on the outside loop
    for ( k = 0; k < VEC_WIDTH; k ++ ) {

      p0 = pj + idx[k];

      // accumulate j1
      for ( k1 = 0; k1 < ORDER; k1++ ) {
        p0[k1].j1 += vwl1[k1].v[k] ;
      }

      // accumulate j2, j3
      for ( k1 = 0; k1 < NP; k1++ ) {
        p0[k1].j2 += j2[k1].v[k];
        p0[k1].j3 += j3[k1].v[k];
      }

    }

  }

}

#endif

/*
  mk. I - Transpose currents, add 1 particle at a time
        Small ~10% improvement from reference implementation
*/

#if 1

#warning Using mk. I 1D - algorithm

#if ( ORDER == 1 )

#define ACC_CURRENT1D( k, v, p ) {                                      \
  float *p0 = pj + idx[k];                                              \
  __m512 v0, j0;                                                        \
  v0 = _mm512_mask_expandloadu_ps( v0, 0x0077, (void *) (p0)      );    \
  j0 = _mm512_permute4f128_ps(                  v[0], p );              \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0070, v[1], p );              \
  v0 = _mm512_mask_add_ps( v0, 0x0077 , j0, v0 );                       \
  _mm512_mask_compressstoreu_ps( (void *) (p0),          0x0077, v0);   \
}

#elif ( ORDER == 2 )

#define ACC_CURRENT1D( k, v, p ) {                                      \
  float *p0 = pj + idx[k];                                              \
  __m512 v0, j0;                                                        \
  v0 = _mm512_mask_expandloadu_ps( v0, 0x0777, (void *) (p0)      );    \
  j0 = _mm512_permute4f128_ps(                  v[0], p );              \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0070, v[1], p );              \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0700, v[2], p );              \
  v0 = _mm512_mask_add_ps( v0, 0x0777 , j0, v0 );                       \
  _mm512_mask_compressstoreu_ps( (void *) (p0),          0x0777, v0);   \
}

#elif ( ORDER == 3 )

#define ACC_CURRENT1D( k, v, p ) {                                      \
  float *p0 = pj + idx[k];                                              \
  __m512 v0, j0;                                                        \
  v0 = _mm512_mask_expandloadu_ps( v0, 0x7777, (void *) (p0)      );    \
  j0 = _mm512_permute4f128_ps(                  v[0], p );              \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0070, v[1], p );              \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0700, v[2], p );              \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x7000, v[3], p );              \
  v0 = _mm512_mask_add_ps( v0, 0x7777 , j0, v0 );                       \
  _mm512_mask_compressstoreu_ps( (void *) (p0),          0x7777, v0);   \
}

#elif ( ORDER == 4 )

#define ACC_CURRENT1D( k, v, p ) { \
  float *p0 = pj + idx[k];                                              \
  __m512 v0, v1, j0, j1;                                                \
  v0 = _mm512_mask_expandloadu_ps( v0, 0x7777, (void *) (p0)      );    \
  v1 = _mm512_loadu_ps( (void *) (p0 + 12) );                           \
  j0 = _mm512_permute4f128_ps(                  v[0], p );              \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0070, v[1], p );              \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0700, v[2], p );              \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x7000, v[3], p );              \
  v0 = _mm512_mask_add_ps( v0, 0x7777 , j0, v0 );                       \
  _mm512_mask_compressstoreu_ps( (void *) (p0),          0x7777, v0);   \
  j1 = _mm512_permute4f128_ps(                  v[4], p );              \
  v1 = _mm512_mask_add_ps( v1, 0x0007 , j1, v1 );                       \
  _mm512_mask_storeu_ps( (void *) (p0 + 12),          0x0007, v1);      \
}

#else
#error Unsupported interpolation order
#endif


void DEP_CURRENT_1D
(float * const current, int const * const size, int const * const offset,
                       float * const norm, t_split_buf1D * const part)
{


  __m512 vx0, vx1, vq, vvy, vvz;

  __m512 vqnx, vqvy, vqvz;
  __m512 vs0x[NP], vs1x[NP];

  int i, np;

  __m512 const c1_2  = _mm512_set1_ps( 0.5 );

  float* const pj =  current + ( offset[0] - OFFSET ) * 3 ;

  __m512 const vnorm1 = _mm512_set1_ps(norm[0]);

  np = part -> np;

  // If number of particles in buffer is not multiple of VEC_WIDTH add dummy particles to the end

  if ( np % VEC_WIDTH ) {
    for( i = 0; i < VEC_WIDTH - np%VEC_WIDTH; i ++ ) {
       int k = np + i;

       part->x0[k] = part->x1[k] = 0.0f;
       part-> q[k] = part->vy[k] = part->vz[k] = 0.0f;

       part->ix[k] = 1;
    }
  }

  for( i = 0; i < np; i += VEC_WIDTH ) {

    int k1;

    __m512 vwl1[NP];

    __m512i vix;
    DECLARE_ALIGNED_64( int idx[VEC_WIDTH] );
    __m512 a[NP], b[NP], c[NP], d[NP];

    // load VEC_WIDTH particles
    LOAD16P1D( part, i, vx0, vx1, vq, vvy, vvz, vix )

    // Store cell index
    vix = _mm512_add_epi32( vix, _mm512_add_epi32( vix, vix ) );
    _mm512_store_epi32( (__m512i *) idx, vix );

    // Get splines
    SPLINE( vx0, vs0x );
    SPLINE( vx1, vs1x );

    vqnx = _mm512_mul_ps( vq, vnorm1 );
    vqvy = _mm512_mul_ps( vq, vvy );
    vqvz = _mm512_mul_ps( vq, vvz );

    // get longitudinal weights
    WL( vqnx, vx0, vx1, vwl1 ); vwl1[NP-1] = _mm512_setzero_ps();

    // get currents and transpose vectors
    for ( k1 = 0; k1 < NP; k1++ ) {
      __m512 vwp1, j1, j2, j3;

      vwp1 = _mm512_mul_ps( c1_2, _mm512_add_ps(vs0x[k1], vs1x[k1]) );

      j1  = vwl1[k1];
      j2  = _mm512_mul_ps( vqvy, vwp1 );
      j3  = _mm512_mul_ps( vqvz, vwp1 );

      // Do a 16x4 transpose, ignoring 4th current component
      __m512 t0 = _mm512_mask_swizzle_ps( j1, 0xAAAA, j2, _MM_SWIZ_REG_CDAB );
      __m512 t1 = _mm512_mask_swizzle_ps( j3, 0xAAAA, j3, _MM_SWIZ_REG_CDAB );
      __m512 t2 = _mm512_mask_swizzle_ps( j2, 0x5555, j1, _MM_SWIZ_REG_CDAB );
      __m512 t3 = _mm512_mask_swizzle_ps( j3, 0x5555, j3, _MM_SWIZ_REG_CDAB );
      a[k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t0), 0xAA, _mm512_castps_pd(t1), _MM_SWIZ_REG_CDAB )));
      b[k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t2), 0xAA, _mm512_castps_pd(t3), _MM_SWIZ_REG_CDAB )));
      c[k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t1), 0x55, _mm512_castps_pd(t0), _MM_SWIZ_REG_CDAB )));
      d[k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t3), 0x55, _mm512_castps_pd(t2), _MM_SWIZ_REG_CDAB )));

    }

    ACC_CURRENT1D(  0, a, _MM_PERM_AAAA )
    ACC_CURRENT1D(  1, b, _MM_PERM_AAAA )
    ACC_CURRENT1D(  2, c, _MM_PERM_AAAA )
    ACC_CURRENT1D(  3, d, _MM_PERM_AAAA )
    ACC_CURRENT1D(  4, a, _MM_PERM_BBBB )
    ACC_CURRENT1D(  5, b, _MM_PERM_BBBB )
    ACC_CURRENT1D(  6, c, _MM_PERM_BBBB )
    ACC_CURRENT1D(  7, d, _MM_PERM_BBBB )
    ACC_CURRENT1D(  8, a, _MM_PERM_CCCC )
    ACC_CURRENT1D(  9, b, _MM_PERM_CCCC )
    ACC_CURRENT1D( 10, c, _MM_PERM_CCCC )
    ACC_CURRENT1D( 11, d, _MM_PERM_CCCC )
    ACC_CURRENT1D( 12, a, _MM_PERM_DDDD )
    ACC_CURRENT1D( 13, b, _MM_PERM_DDDD )
    ACC_CURRENT1D( 14, c, _MM_PERM_DDDD )
    ACC_CURRENT1D( 15, d, _MM_PERM_DDDD )

  }

}

#undef ACC_CURRENT1D

#endif

/****************************************************************************************
  2D current deposition
*****************************************************************************************/

#if 0

/**
 * Reference implementation
 *
 * For each particle, deposit one current component at a time
 *
 * This is the slowest
 */

void DEP_CURRENT_2D
(float * const current, int const * const size, int const * const offset,
                       float * const norm, t_split_buf2D * const part)
{

  typedef struct Current { float j1, j2, j3; } t_current;
  t_current *pjpart, *p0;


  fvec j3[NP][NP];
  fvec vwp1[NP], vwp2[NP];
  fvec vwl1[ORDER], vwl2[ORDER];

  __m512 vx0, vx1, vy0, vy1, vq, vvz;

  DECLARE_ALIGNED_64( int idx[VEC_WIDTH] );

  __m512 vqnx, vqny, vqvz;
  __m512 vs0x[NP], vs1x[NP], vs0y[NP], vs1y[NP];

  int np;
  int i, k, k1, k2;

  __m512 const c1_3 = _mm512_set1_ps( 1.0/3.0 );
  __m512 const c1_2  = _mm512_set1_ps( 0.5 );

  int const Dy = size[0];
  t_current* const pj = (t_current *) (current + ( offset[0] - OFFSET ) * 3 +
                                                 ( offset[1] - OFFSET ) * 3 * Dy );

  // Setting this to 1/4 removes 1 multiplication from wl2
  __m512 const vnorm1 = _mm512_set1_ps(norm[0]);
  __m512 const vnorm2 = _mm512_set1_ps(norm[1]);

  np = part -> np;

  // If number of particles in buffer is not multiple of VEC_WIDTH add dummy particles to the end

  if ( np % VEC_WIDTH != 0 ) {
    for( i = 0; i < VEC_WIDTH - np%VEC_WIDTH; i ++ ) {
       int k = np + i;

       part->x0[k] = part->x1[k] = 0.0f;
       part->y0[k] = part->y1[k] = 0.0f;
       part-> q[k] = part->vz[k] = 0.0f;

       part->ix[k] = part->iy[k] = 1;
    }
  }

  // Each virtual particle uses 8 doubles, so 4 vp = 32 doubles
  for( i = 0; i < np; i += VEC_WIDTH ) {

    __m512i vix, viy;

    // load VEC_WIDTH particles
    LOAD16P2D( part, i, vx0, vx1, vy0, vy1, vq, vvz, vix, viy )

    // Store cell index
    _mm512_store_epi32( (__m512i *) idx,
            _mm512_add_epi32( vix, _mm512_mullo_epi32( viy, _mm512_set1_epi32( Dy ) ) ) );

    // Get splines
    SPLINE( vx0, vs0x );
    SPLINE( vx1, vs1x );
    SPLINE( vy0, vs0y );
    SPLINE( vy1, vs1y );

    vqnx = _mm512_mul_ps( vq, vnorm1 );
    vqny = _mm512_mul_ps( vq, vnorm2 );
    vqvz = _mm512_mul_ps( vq, vvz);
    vqvz = _mm512_mul_ps( vqvz, c1_3 );

    // get longitudinal weights
    WL( vqnx, vx0, vx1, (__m512 *) vwl1 );
    WL( vqny, vy0, vy1, (__m512 *) vwl2 );

    // get perpendicular weights
    for( k = 0; k < NP; k++ ) {
      vwp1[k].v16 = _mm512_add_ps(vs0y[k], vs1y[k]);
      vwp2[k].v16 = _mm512_add_ps(vs0x[k], vs1x[k]);
    }

    // get j3 current
    for( k2 = 0; k2 < NP; k2++ ) {
      for ( k1 = 0; k1 < NP; k1++ ) {
        __m512 s00, s10, tmp1, tmp2;

		s00  = _mm512_mul_ps( vs0x[k1], vs0y[k2] );
		tmp1 = _mm512_fmadd_ps( vs1y[k2], vs1x[k1], s00 );  // tmp1 = s0x*s0y + s1x*s1y

		s10  = _mm512_mul_ps( vs0x[k1], vs1y[k2] );
		tmp2 = _mm512_fmadd_ps( vs0y[k2], vs1x[k1], s10 );  // tmp2 = s0x*s1y + s1x*s0y

		tmp1 = _mm512_fmadd_ps( tmp2, c1_2, tmp1 );       // tmp1 = tmp1 + 0.5*tmp2

		j3[k1][k2].v16 = _mm512_mul_ps( vqvz, tmp1 );          // j3 = vqvz * tmp1
      }
    }

    // Loop by particle on the outside loop
    for ( k = 0; k < VEC_WIDTH; k ++ ) {

      // This is not vectorially because it's a 64bit op
      pjpart = pj + idx[k];

	  // accumulate j1
	  for( k2 = 0; k2 < NP; k2++ ) {
		p0 = pjpart + k2*Dy;
		for ( k1 = 0; k1 < ORDER; k1++ ) {
		  p0[k1].j1 += vwl1[k1].v[k] * vwp1[k2].v[k];
		}
	  }

	  // accumulate j2 - making k2 the outside loop gives marginal perf. gain
	  for( k2 = 0; k2 < ORDER; k2++ ) {
		p0 = pjpart + k2*Dy;
		for ( k1 = 0; k1 < NP; k1++ ) {
		   p0[k1].j2 += vwl2[k2].v[k] * vwp2[k1].v[k];
		}
	  }

	  // accumulate j3
	  for( k2 = 0; k2 < NP; k2++ ) {
		p0 = pjpart + k2*Dy;
		for ( k1 = 0; k1 < NP; k1++ ) {
		   p0[k1].j3 += (j3[k1][k2]).v[k];
		}
	  }

    }

  }

}
#endif


#if 1

/**
 * mk. I
 *
 * Transpose currents, then for each particle deposit one current line at a time
 *
 * This is the recommended version
 */


#warning Using mk. I 2D algorithm

#if ( ORDER == 1 )

#define ACC_CURRENT2D( k, v, p ) { \
  float * const line = pj + idx[k]; \
  for( int k2 = 0; k2 < 2; k2++) { \
    __m512 va, ja; \
    va = _mm512_mask_expandloadu_ps( va, 0x0077, &line[ k2 * Dy ] ); \
    ja = _mm512_permute4f128_ps(                  v[k2][0], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[k2][1], p ); \
    \
    va = _mm512_mask_add_ps( va, 0x0077 , ja, va ); \
    _mm512_mask_compressstoreu_ps( &line[ k2 * Dy ], 0x0077, va); \
  } \
}



#elif ( ORDER == 2 )

#define ACC_CURRENT2D( k, v, p ) { \
  float * const line = pj + idx[k]; \
  for( int k2 = 0; k2 < 3; k2++) { \
    __m512 va, ja; \
    va = _mm512_mask_expandloadu_ps( va, 0x0777, &line[ k2 * Dy ] ); \
    ja = _mm512_permute4f128_ps(                  v[k2][0], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[k2][1], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0700, v[k2][2], p ); \
    \
    va = _mm512_mask_add_ps( va, 0x0777 , ja, va ); \
    _mm512_mask_compressstoreu_ps( &line[ k2 * Dy ], 0x0777, va); \
  } \
}

#elif ( ORDER == 3 )

#warning ORDER 3

#define ACC_CURRENT2D( k, v, p ) { \
  float * const line = pj + idx[k]; \
  for( int k2 = 0; k2 < 4; k2++) { \
    __m512 va, ja; \
    va = _mm512_mask_expandloadu_ps( va, 0x7777, &line[ k2 * Dy ] ); \
    ja = _mm512_permute4f128_ps(                  v[k2][0], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[k2][1], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0700, v[k2][2], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x7000, v[k2][3], p ); \
    \
    va = _mm512_mask_add_ps( va, 0x7777 , ja, va ); \
    _mm512_mask_compressstoreu_ps( &line[ k2 * Dy ], 0x7777, va); \
  } \
}

#elif ( ORDER == 4 )

#define ACC_CURRENT2D( k, v, p ) { \
  float * const line = pj + idx[k]; \
  for( int k2 = 0; k2 < 5; k2++) { \
    __m512 va, ja, vb, jb; \
    va = _mm512_mask_expandloadu_ps( va, 0x7777, &line[ k2 * Dy ] ); \
    vb = _mm512_loadu_ps( &line[ k2 * Dy + 12] ); \
    ja = _mm512_permute4f128_ps(                  v[k2][0], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[k2][1], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0700, v[k2][2], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x7000, v[k2][3], p ); \
    jb = _mm512_permute4f128_ps(                  v[k2][4], p ); \
    \
    va = _mm512_mask_add_ps( va, 0x7777 , ja, va ); \
    vb = _mm512_mask_add_ps( vb, 0x0007 , jb, vb ); \
    _mm512_mask_compressstoreu_ps( &line[ k2 * Dy ], 0x7777, va); \
    _mm512_mask_storeu_ps(  &line[ k2 * Dy + 12 ], 0x0007, vb); \
  } \
}


#else
#error Unsupported interpolation order
#endif


void DEP_CURRENT_2D
(float * restrict const current, int const * restrict const size, int const * restrict const offset,
                       float * restrict const norm, t_split_buf2D * restrict const part)
{

  __m512 vwp1[NP], vwp2[NP];
  __m512 vwl1[NP], vwl2[NP];

  __m512 vx0, vx1, vy0, vy1, vq, vvz;

  DECLARE_ALIGNED_64( int idx[VEC_WIDTH] );

  __m512 vqnx, vqny, vqvz;
  __m512 vs0x[NP], vs1x[NP], vs0y[NP], vs1y[NP];

  __m512 const c1_3 = _mm512_set1_ps( 1.0f/3.0f );
  __m512 const c1_2  = _mm512_set1_ps( 0.5f );

  // Current array has 3 field components
  float* const pj = current + 3 * ( ( offset[0] - OFFSET ) +
    ( offset[1] - OFFSET ) * size[0] );

  // Normalization for current
  __m512 const vnorm1 = _mm512_set1_ps(norm[0]);
  __m512 const vnorm2 = _mm512_set1_ps(norm[1]);

  int const Dy = 3*size[0];
  __m512i const vDy = _mm512_set1_epi32( Dy );

  int i;
  int np = part -> np;

  // If number of particles in buffer is not multiple of VEC_WIDTH add dummy particles to the end
  if ( np % VEC_WIDTH != 0 ) {
    for( i = 0; i < VEC_WIDTH - np%VEC_WIDTH; i ++ ) {
      int k = np + i;

      part->x0[k] = part->x1[k] = 0.0f;
      part->y0[k] = part->y1[k] = 0.0f;
      part-> q[k] = part->vz[k] = 0.0f;

      part->ix[k] = part->iy[k] = 1;
    }
  }

  for( i = 0; i < np; i += VEC_WIDTH ) {

    int k, k1, k2;
    __m512i vix, viy;

    // load VEC_WIDTH particles
    LOAD16P2D( part, i, vx0, vx1, vy0, vy1, vq, vvz, vix, viy )

    // Store cell index
    // idx = 3 * vix + 3 * size[0] * viy
    vix = _mm512_add_epi32( vix, _mm512_add_epi32( vix, vix ) );
    viy = _mm512_mullo_epi32( viy, vDy );
    _mm512_store_epi32( (__m512i *) idx, _mm512_add_epi32( vix, viy ) );

    // Get splines
    SPLINE( vx0, vs0x );
    SPLINE( vx1, vs1x );
    SPLINE( vy0, vs0y );
    SPLINE( vy1, vs1y );

    vqnx = _mm512_mul_ps( vq, vnorm1 );
    vqny = _mm512_mul_ps( vq, vnorm2 );
    vqvz = _mm512_mul_ps( vq, vvz );
    vqvz = _mm512_mul_ps( vqvz, c1_3 );

    // get longitudinal weights
    WL( vqnx, vx0, vx1, vwl1 ); vwl1[NP-1] = _mm512_setzero_ps();
    WL( vqny, vy0, vy1, vwl2 ); vwl2[NP-1] = _mm512_setzero_ps();

    // get perpendicular weights
    for( k = 0; k < NP; k++ ) {
      vwp1[k] = _mm512_add_ps(vs0y[k], vs1y[k]);
      vwp2[k] = _mm512_add_ps(vs0x[k], vs1x[k]);
    }


    __m512 a[NP][NP], b[NP][NP], c[NP][NP], d[NP][NP];

    for( k2 = 0; k2 < NP; k2++ ) {
      for ( k1 = 0; k1 < NP; k1++ ) {
        __m512 j1, j2, j3;

        j1 = _mm512_mul_ps( vwl1[k1] , vwp1[k2] );
        j2 = _mm512_mul_ps( vwl2[k2] , vwp2[k1] );

        __m512 s00, s10, tmp1, tmp2;

        s00  = _mm512_mul_ps( vs0x[k1], vs0y[k2] );
        tmp1 = _mm512_fmadd_ps( vs1y[k2], vs1x[k1], s00 );

        s10  = _mm512_mul_ps( vs0x[k1], vs1y[k2] );
        tmp2 = _mm512_fmadd_ps( vs0y[k2], vs1x[k1], s10 );

        tmp1 = _mm512_fmadd_ps( tmp2, c1_2, tmp1 );

        j3 = _mm512_mul_ps( vqvz, tmp1 );

        // Do a 16x4 transpose, ignoring 4th current component

        __m512 t0 = _mm512_mask_swizzle_ps( j1, 0xAAAA, j2, _MM_SWIZ_REG_CDAB );
        __m512 t1 = _mm512_mask_swizzle_ps( j3, 0xAAAA, j3, _MM_SWIZ_REG_CDAB );
        __m512 t2 = _mm512_mask_swizzle_ps( j2, 0x5555, j1, _MM_SWIZ_REG_CDAB );
        __m512 t3 = _mm512_mask_swizzle_ps( j3, 0x5555, j3, _MM_SWIZ_REG_CDAB );
        a[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t0), 0xAA, _mm512_castps_pd(t1), _MM_SWIZ_REG_CDAB )));
        b[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t2), 0xAA, _mm512_castps_pd(t3), _MM_SWIZ_REG_CDAB )));
        c[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t1), 0x55, _mm512_castps_pd(t0), _MM_SWIZ_REG_CDAB )));
        d[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t3), 0x55, _mm512_castps_pd(t2), _MM_SWIZ_REG_CDAB )));
      }
    }

    ACC_CURRENT2D(  0, a, _MM_PERM_AAAA )
    ACC_CURRENT2D(  1, b, _MM_PERM_AAAA )
    ACC_CURRENT2D(  2, c, _MM_PERM_AAAA )
    ACC_CURRENT2D(  3, d, _MM_PERM_AAAA )
    ACC_CURRENT2D(  4, a, _MM_PERM_BBBB )
    ACC_CURRENT2D(  5, b, _MM_PERM_BBBB )
    ACC_CURRENT2D(  6, c, _MM_PERM_BBBB )
    ACC_CURRENT2D(  7, d, _MM_PERM_BBBB )
    ACC_CURRENT2D(  8, a, _MM_PERM_CCCC )
    ACC_CURRENT2D(  9, b, _MM_PERM_CCCC )
    ACC_CURRENT2D( 10, c, _MM_PERM_CCCC )
    ACC_CURRENT2D( 11, d, _MM_PERM_CCCC )
    ACC_CURRENT2D( 12, a, _MM_PERM_DDDD )
    ACC_CURRENT2D( 13, b, _MM_PERM_DDDD )
    ACC_CURRENT2D( 14, c, _MM_PERM_DDDD )
    ACC_CURRENT2D( 15, d, _MM_PERM_DDDD )

  }

}

#undef ACC_CURRENT2D

#endif


#if 0

/**
 * mk. II - Same as mk. I with manually unrolled loops
 *
 * There is no noticeable performance improvement over mk. I
 */

#warning Using mk. II 2D algorithm

#if ( ORDER == 1 )

#define ACC_CURRENT2D( k, v, p ) { \
  float *p0, *p1;        \
  p0 = pj + idx[k];      \
  p1 = p0 + Dy;          \
  __m512 v0, v1, j0, j1; \
  v0 = _mm512_mask_expandloadu_ps( v0, 0x0077, (void *) (p0) ); \
  v1 = _mm512_mask_expandloadu_ps( v1, 0x0077, (void *) (p1) ); \
  j0 = _mm512_permute4f128_ps(                  v[0][0], p ); \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0070, v[0][1], p ); \
  j1 = _mm512_permute4f128_ps(                  v[1][0], p ); \
  j1 = _mm512_mask_permute4f128_ps( j1, 0x0070, v[1][1], p ); \
  v0 = _mm512_mask_add_ps( v0, 0x0077 , j0, v0 ); \
  v1 = _mm512_mask_add_ps( v1, 0x0077 , j1, v1 ); \
  _mm512_mask_compressstoreu_ps( (void *) (p0), 0x0077, v0); \
  _mm512_mask_compressstoreu_ps( (void *) (p1), 0x0077, v1); \
}


#elif ( ORDER == 2 )

#define ACC_CURRENT2D( k, v, p ) { \
  float *p0, *p1, *p2; \
  p0 = pj + idx[k];  \
  p1 = p0 + Dy;      \
  p2 = p0 + 2*Dy;    \
  __m512 v0, v1, v2; \
  __m512 j0, j1, j2; \
  v0 = _mm512_mask_expandloadu_ps( v0, 0x0777, (void *) (p0) ); \
  v1 = _mm512_mask_expandloadu_ps( v1, 0x0777, (void *) (p1) ); \
  v2 = _mm512_mask_expandloadu_ps( v2, 0x0777, (void *) (p2) ); \
  j0 = _mm512_permute4f128_ps(                  v[0][0], p ); \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0070, v[0][1], p ); \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0700, v[0][2], p ); \
  j1 = _mm512_permute4f128_ps(                  v[1][0], p ); \
  j1 = _mm512_mask_permute4f128_ps( j1, 0x0070, v[1][1], p ); \
  j1 = _mm512_mask_permute4f128_ps( j1, 0x0700, v[1][2], p ); \
  j2 = _mm512_permute4f128_ps(                  v[2][0], p ); \
  j2 = _mm512_mask_permute4f128_ps( j2, 0x0070, v[2][1], p ); \
  j2 = _mm512_mask_permute4f128_ps( j2, 0x0700, v[2][2], p ); \
  v0 = _mm512_mask_add_ps( v0, 0x0777 , j0, v0 ); \
  v1 = _mm512_mask_add_ps( v1, 0x0777 , j1, v1 ); \
  v2 = _mm512_mask_add_ps( v2, 0x0777 , j2, v2 ); \
  _mm512_mask_compressstoreu_ps( (void *) (p0), 0x0777, v0); \
  _mm512_mask_compressstoreu_ps( (void *) (p1), 0x0777, v1); \
  _mm512_mask_compressstoreu_ps( (void *) (p2), 0x0777, v2); \
}

#elif ( ORDER == 3 )

#define ACC_CURRENT2D( k, v, p ) { \
  float *p0, *p1, *p2, *p3; \
  p0 = pj + idx[k];              \
  p1 = p0 + Dy;                  \
  p2 = p0 + 2*Dy;                \
  p3 = p0 + 3*Dy;                \
  __m512 v0, v1, v2, v3;         \
  __m512 j0, j1, j2, j3;         \
  v0 = _mm512_mask_expandloadu_ps( v0, 0x7777, (void *) (p0) ); \
  v1 = _mm512_mask_expandloadu_ps( v1, 0x7777, (void *) (p1) ); \
  v2 = _mm512_mask_expandloadu_ps( v2, 0x7777, (void *) (p2) ); \
  v3 = _mm512_mask_expandloadu_ps( v3, 0x7777, (void *) (p3) ); \
  j0 = _mm512_permute4f128_ps(                  v[0][0], p ); \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0070, v[0][1], p ); \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x0700, v[0][2], p ); \
  j0 = _mm512_mask_permute4f128_ps( j0, 0x7000, v[0][3], p ); \
  j1 = _mm512_permute4f128_ps(                  v[1][0], p ); \
  j1 = _mm512_mask_permute4f128_ps( j1, 0x0070, v[1][1], p ); \
  j1 = _mm512_mask_permute4f128_ps( j1, 0x0700, v[1][2], p ); \
  j1 = _mm512_mask_permute4f128_ps( j1, 0x7000, v[1][3], p ); \
  j2 = _mm512_permute4f128_ps(                  v[2][0], p ); \
  j2 = _mm512_mask_permute4f128_ps( j2, 0x0070, v[2][1], p ); \
  j2 = _mm512_mask_permute4f128_ps( j2, 0x0700, v[2][2], p ); \
  j2 = _mm512_mask_permute4f128_ps( j2, 0x7000, v[2][3], p ); \
  j3 = _mm512_permute4f128_ps(                  v[3][0], p ); \
  j3 = _mm512_mask_permute4f128_ps( j3, 0x0070, v[3][1], p ); \
  j3 = _mm512_mask_permute4f128_ps( j3, 0x0700, v[3][2], p ); \
  j3 = _mm512_mask_permute4f128_ps( j3, 0x7000, v[3][3], p ); \
  v0 = _mm512_mask_add_ps( v0, 0x7777 , j0, v0 ); \
  v1 = _mm512_mask_add_ps( v1, 0x7777 , j1, v1 ); \
  v2 = _mm512_mask_add_ps( v2, 0x7777 , j2, v2 ); \
  v3 = _mm512_mask_add_ps( v3, 0x7777 , j3, v3 ); \
  _mm512_mask_compressstoreu_ps( (void *) (p0), 0x7777, v0); \
  _mm512_mask_compressstoreu_ps( (void *) (p1), 0x7777, v1); \
  _mm512_mask_compressstoreu_ps( (void *) (p2), 0x7777, v2); \
  _mm512_mask_compressstoreu_ps( (void *) (p3), 0x7777, v3); \
}


#elif ( ORDER == 4 )

// This would be very large, use mk. I instead

#define ACC_CURRENT2D( k, v, p ) { \
  float * const pa = pj + idx[k]; \
  \
  for( int j = 0; j < 5; j++) { \
    __m512 va, ja, vb, jb; \
    va = _mm512_mask_expandloadu_ps( va, 0x7777, (void *) (pa + j*Dy) ); \
    vb = _mm512_loadu_ps( (void *) (pa + j*Dy + 12) ); \
    ja = _mm512_permute4f128_ps(                  v[j][0], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[j][1], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0700, v[j][2], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x7000, v[j][3], p ); \
    jb = _mm512_permute4f128_ps(                  v[j][4], p ); \
    \
    va = _mm512_mask_add_ps( va, 0x7777 , ja, va ); \
    vb = _mm512_mask_add_ps( vb, 0x0007 , jb, vb ); \
    _mm512_mask_compressstoreu_ps( (void *) (pa + j*Dy), 0x7777, va); \
    _mm512_mask_storeu_ps(    (void *) (pa + j*Dy + 12), 0x0007, vb); \
  } \
}


#else
#error Unsupported interpolation order
#endif


void DEP_CURRENT_2D
(float * restrict const current, int const * restrict const size, int const * restrict const offset,
                       float * restrict const norm, t_split_buf2D * restrict const part)
{

  __m512 vwp1[NP], vwp2[NP];
  __m512 vwl1[NP], vwl2[NP];

  __m512 vx0, vx1, vy0, vy1, vq, vvz;

  DECLARE_ALIGNED_64( int idx[VEC_WIDTH] );

  __m512 vqnx, vqny, vqvz;
  __m512 vs0x[NP], vs1x[NP], vs0y[NP], vs1y[NP];

  __m512 const c1_3 = _mm512_set1_ps( 1.0f/3.0f );
  __m512 const c1_2  = _mm512_set1_ps( 0.5f );

  // Current array has 3 field components
  float* const pj = current + 3 * ( ( offset[0] - OFFSET ) +
                                    ( offset[1] - OFFSET ) * size[0] );

  // Normalization for current
  __m512 const vnorm1 = _mm512_set1_ps(norm[0]);
  __m512 const vnorm2 = _mm512_set1_ps(norm[1]);

  int const Dy = 3*size[0];
  __m512i const vDy = _mm512_set1_epi32( Dy );

  int i;
  int np = part -> np;

  // If number of particles in buffer is not multiple of VEC_WIDTH add dummy particles to the end

  if ( np % VEC_WIDTH != 0 ) {
    for( i = 0; i < VEC_WIDTH - np%VEC_WIDTH; i ++ ) {
       int k = np + i;

       part->x0[k] = part->x1[k] = 0.0f;
       part->y0[k] = part->y1[k] = 0.0f;
       part-> q[k] = part->vz[k] = 0.0f;

       part->ix[k] = part->iy[k] = 1;
    }
  }

  for( i = 0; i < np; i += VEC_WIDTH ) {

    int k, k1, k2;
    __m512i vix, viy;

    // load VEC_WIDTH particles
    LOAD16P2D( part, i, vx0, vx1, vy0, vy1, vq, vvz, vix, viy )

    // Store cell index
    // idx = 3 * vix + 3 * size[0] * viy
    vix = _mm512_add_epi32( vix, _mm512_add_epi32( vix, vix ) );
    viy = _mm512_mullo_epi32( viy, vDy );
    _mm512_store_epi32( (__m512i *) idx, _mm512_add_epi32( vix, viy ) );

    // Get splines
    SPLINE( vx0, vs0x );
    SPLINE( vx1, vs1x );
    SPLINE( vy0, vs0y );
    SPLINE( vy1, vs1y );

    vqnx = _mm512_mul_ps( vq, vnorm1 );
    vqny = _mm512_mul_ps( vq, vnorm2 );
    vqvz = _mm512_mul_ps( vq, vvz);
    vqvz = _mm512_mul_ps( vqvz, c1_3 );

    // get longitudinal weights
    WL( vqnx, vx0, vx1, vwl1 ); vwl1[NP-1] = _mm512_setzero_ps();
    WL( vqny, vy0, vy1, vwl2 ); vwl2[NP-1] = _mm512_setzero_ps();

    // get perpendicular weights
    for( k = 0; k < NP; k++ ) {
      vwp1[k] = _mm512_add_ps(vs0y[k], vs1y[k]);
      vwp2[k] = _mm512_add_ps(vs0x[k], vs1x[k]);
    }


    __m512 a[NP][NP], b[NP][NP], c[NP][NP], d[NP][NP];

    for( k2 = 0; k2 < NP; k2++ ) {
      for ( k1 = 0; k1 < NP; k1++ ) {
         __m512 j1, j2, j3;

         j1 = _mm512_mul_ps( vwl1[k1] , vwp1[k2] );
         j2 = _mm512_mul_ps( vwl2[k2] , vwp2[k1] );

     __m512 s00, s10, tmp1, tmp2;

     s00  = _mm512_mul_ps( vs0x[k1], vs0y[k2] );
     tmp1 = _mm512_fmadd_ps( vs1y[k2], vs1x[k1], s00 );

     s10  = _mm512_mul_ps( vs0x[k1], vs1y[k2] );
     tmp2 = _mm512_fmadd_ps( vs0y[k2], vs1x[k1], s10 );

     tmp1 = _mm512_fmadd_ps( tmp2, c1_2, tmp1 );

     j3 = _mm512_mul_ps( vqvz, tmp1 );

     // Do a 16x4 transpose, ignoring 4th current component

     __m512 t0 = _mm512_mask_swizzle_ps( j1, 0xAAAA, j2, _MM_SWIZ_REG_CDAB );
     __m512 t1 = _mm512_mask_swizzle_ps( j3, 0xAAAA, j3, _MM_SWIZ_REG_CDAB );
     __m512 t2 = _mm512_mask_swizzle_ps( j2, 0x5555, j1, _MM_SWIZ_REG_CDAB );
     __m512 t3 = _mm512_mask_swizzle_ps( j3, 0x5555, j3, _MM_SWIZ_REG_CDAB );
     a[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t0), 0xAA, _mm512_castps_pd(t1), _MM_SWIZ_REG_CDAB )));
     b[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t2), 0xAA, _mm512_castps_pd(t3), _MM_SWIZ_REG_CDAB )));
     c[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t1), 0x55, _mm512_castps_pd(t0), _MM_SWIZ_REG_CDAB )));
     d[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t3), 0x55, _mm512_castps_pd(t2), _MM_SWIZ_REG_CDAB )));
    }
  }


    ACC_CURRENT2D(  0, a, _MM_PERM_AAAA )
    ACC_CURRENT2D(  1, b, _MM_PERM_AAAA )
    ACC_CURRENT2D(  2, c, _MM_PERM_AAAA )
    ACC_CURRENT2D(  3, d, _MM_PERM_AAAA )
    ACC_CURRENT2D(  4, a, _MM_PERM_BBBB )
    ACC_CURRENT2D(  5, b, _MM_PERM_BBBB )
    ACC_CURRENT2D(  6, c, _MM_PERM_BBBB )
    ACC_CURRENT2D(  7, d, _MM_PERM_BBBB )
    ACC_CURRENT2D(  8, a, _MM_PERM_CCCC )
    ACC_CURRENT2D(  9, b, _MM_PERM_CCCC )
    ACC_CURRENT2D( 10, c, _MM_PERM_CCCC )
    ACC_CURRENT2D( 11, d, _MM_PERM_CCCC )
    ACC_CURRENT2D( 12, a, _MM_PERM_DDDD )
    ACC_CURRENT2D( 13, b, _MM_PERM_DDDD )
    ACC_CURRENT2D( 14, c, _MM_PERM_DDDD )
    ACC_CURRENT2D( 15, d, _MM_PERM_DDDD )

  }

}

#undef ACC_CURRENT2D

#endif

/****************************************************************************************
  3D current deposition
****************************************************************************************/

/**
 * Reference implementation
 *
 * For each particle, accumulate 1 field component at a time.
 *
 * This is the slowest implementation.
 */

#if 0

#warning Using reference 3D algorithm

void DEP_CURRENT_3D
(float * const  current, int const * const  size, int const * const  offset,
                       float * const norm, t_split_buf3D * const part)
{

  typedef struct Current { float j1, j2, j3; } t_current;

  __m512 vx0, vx1, vy0, vy1,  vz0, vz1, vq;

  __m512 vqnx, vqny, vqnz;
  __m512 vs0x[NP], vs1x[NP], vs0y[NP], vs1y[NP], vs0z[NP], vs1z[NP];

  fvec vwl1[ORDER], vwl2[ORDER], vwl3[ORDER];
  fvec vwp1[NP][NP], vwp2[NP][NP], vwp3[NP][NP];

  DECLARE_ALIGNED_64( int idx[VEC_WIDTH] );

  int const Dy = size[0];
  int const Dz = Dy * size[1];

  t_current*  pj = (t_current *) ( current + ( offset[0] - OFFSET  ) * 3 +
                                             ( offset[1] - OFFSET  ) * 3 * Dy +
                                             ( offset[2] - OFFSET  ) * 3 * Dz );

  __m512 const vnorm1 = _mm512_set1_ps(norm[0]);
  __m512 const vnorm2 = _mm512_set1_ps(norm[1]);
  __m512 const vnorm3 = _mm512_set1_ps(norm[2]);

  int i;
  int np = part -> np;

  // If number of particles in buffer is not multiple of VEC_WIDTH add dummy particles to the end
  if ( np % VEC_WIDTH ) {
    for( i = 0; i < VEC_WIDTH - np%VEC_WIDTH; i ++ ) {
       int k = np + i;

       part -> x0[k] = part -> x1[k] = 0.;
       part -> y0[k] = part -> y1[k] = 0.;
       part -> z0[k] = part -> z1[k] = 0.;
       part -> q[k] = 0.;
       part -> ix[k] = part -> iy[k] = part -> iz[k] = 1;
    }
  }

  for( i = 0; i < np; i+=VEC_WIDTH ) {

    int k, k1, k2, k3;
    __m512i vix, viy, viz, vidx;

    // load VEC_WIDTH particles
    LOAD16P3D( part, i, vx0, vx1, vy0, vy1, vz0, vz1, vq, vix, viy, viz )

    // Store cell index
    vidx = _mm512_add_epi32( vix,  _mm512_mullo_epi32( viy, _mm512_set1_epi32( Dy ) ) );
    vidx = _mm512_add_epi32( vidx, _mm512_mullo_epi32( viz, _mm512_set1_epi32( Dz ) ) );

    // Get spline weights
    SPLINE( vx0, vs0x );
    SPLINE( vx1, vs1x );
    SPLINE( vy0, vs0y );
    SPLINE( vy1, vs1y );
    SPLINE( vz0, vs0z );
    SPLINE( vz1, vs1z );

    vqnx = _mm512_mul_ps( vq, vnorm1 );
    vqny = _mm512_mul_ps( vq, vnorm2 );
    vqnz = _mm512_mul_ps( vq, vnorm3 );

    // get longitudinal weights
    WL( vqnx, vx0, vx1, (__m512 *) vwl1 );
    WL( vqny, vy0, vy1, (__m512 *) vwl2 );
    WL( vqnz, vz0, vz1, (__m512 *) vwl3 );

    // get perpendicular weights
    for( k2 = 0; k2 < NP; k2++ ) {
      for( k1 = 0; k1 < NP; k1++ ) {

         const __m512 c1_2 = _mm512_set1_ps(0.5f);
         __m512 tmp1, tmp2;

         // wp1[k2][k1] = s0y[k1]*s0z[k2] + s1y[k1]*s1z[k2] +
         //               0.5*( s0y[k1]*s1z[k2] + s1y[k1]*s0z[k2] )

         tmp1 = _mm512_mul_ps( vs0y[k1], vs0z[k2] );
         tmp2 = _mm512_mul_ps( vs0y[k1], vs1z[k2] );
         tmp1 = _mm512_fmadd_ps( vs1y[k1], vs1z[k2], tmp1 );
         tmp2 = _mm512_fmadd_ps( vs1y[k1], vs0z[k2], tmp2 );

         vwp1[k2][k1].v16 = _mm512_fmadd_ps( c1_2, tmp2, tmp1 );

         // wp2[k2][k1] = s0x[k1]*s0z[k2] + s1x[k1]*s1z[k2] +
         //               0.5*( s0x[k1]*s1z[k2] + s1x[k1]*s0z[k2] )

         tmp1 = _mm512_mul_ps( vs0x[k1], vs0z[k2] );
         tmp2 = _mm512_mul_ps( vs0x[k1], vs1z[k2] );
         tmp1 = _mm512_fmadd_ps( vs1x[k1], vs1z[k2], tmp1 );
         tmp2 = _mm512_fmadd_ps( vs1x[k1], vs0z[k2], tmp2 );

         vwp2[k2][k1].v16 = _mm512_fmadd_ps( c1_2, tmp2, tmp1 );

         // wp3[k2][k1] = s0x[k1]*s0y[k2] + s1x[k1]*s1y[k2] +
         //               0.5*( s0x[k1]*s1y[k2] + s1x[k1]*s0y[k2] )

         tmp1 = _mm512_mul_ps( vs0x[k1], vs0y[k2] );
         tmp2 = _mm512_mul_ps( vs0x[k1], vs1y[k2] );
         tmp1 = _mm512_fmadd_ps( vs1x[k1], vs1y[k2], tmp1 );
         tmp2 = _mm512_fmadd_ps( vs1x[k1], vs0y[k2], tmp2 );

         vwp3[k2][k1].v16 = _mm512_fmadd_ps( c1_2, tmp2, tmp1 );

      }
    }

    // The following code will need to access the individual vidx values
    _mm512_store_epi32( (__m512i *) idx, vidx );

    // Loop by particle on the outside loop
    for ( k = 0; k < VEC_WIDTH; k ++ ) {

      t_current * pjpart = pj + idx[k];

      // accumulate j1
      for( k3 = 0; k3 < NP; k3++ ) {
        for( k2 = 0; k2 < NP; k2++ ) {
          t_current *p0 = pjpart + k2*Dy + k3*Dz;
          for ( k1 = 0; k1 < ORDER; k1++ ) {
            p0[k1].j1 += vwl1[k1].v[k] * vwp1[k3][k2].v[k];
          }
        }
      }

      // accumulate j2
      for( k3 = 0; k3 < NP; k3++ ) {
        for( k2 = 0; k2 < ORDER; k2++ ) {
          t_current *p0 = pjpart + k2*Dy + k3*Dz;
          for ( k1 = 0; k1 < NP; k1++ ) {
            p0[k1].j2 += vwl2[k2].v[k] * vwp2[k3][k1].v[k];
          }
        }
      }

      // accumulate j3
      for( k3 = 0; k3 < ORDER; k3++ ) {
        for( k2 = 0; k2 < NP; k2++ ) {
          t_current *p0 = pjpart + k2*Dy + k3*Dz;
          for ( k1 = 0; k1 < NP; k1++ ) {
            p0[k1].j3 += vwl3[k3].v[k] * vwp3[k2][k1].v[k];
          }
        }
      }

    }

  }
}

#endif

#if 1

/* mk. I

For each vector of particles loop over z coordinate and deposit one current plane at a
time using the same algorithm as 2D mk. I

Note: This will give different roundoff from the other versions because we are not
adding 1 particle at a time.

This is the fastest version

*/

#warning Using mk. I 3D algorithm

#if ( ORDER == 1 )

#define ACC_CURRENT3D( k, v, p ) { \
  float * const pa = pj + idx[k] + k3 * Dz; \
  \
  for( int j = 0; j < 2; j++) { \
    __m512 va, ja; \
    va = _mm512_mask_expandloadu_ps( va, 0x0077, &pa[ j*Dy ] ); \
    ja = _mm512_permute4f128_ps(                  v[j][0], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[j][1], p ); \
    \
    va = _mm512_mask_add_ps( va, 0x0077 , ja, va ); \
    _mm512_mask_compressstoreu_ps( &pa[ j*Dy ], 0x0077, va); \
  } \
}


#elif ( ORDER == 2 )

#define ACC_CURRENT3D( k, v, p ) { \
  float * const pa = pj + idx[k] + k3 * Dz; \
  \
  for( int j = 0; j < 3; j++) { \
    __m512 va, ja; \
    va = _mm512_mask_expandloadu_ps( va, 0x0777, &pa[ j*Dy ] ); \
    ja = _mm512_permute4f128_ps(                  v[j][0], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[j][1], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0700, v[j][2], p ); \
    \
    va = _mm512_mask_add_ps( va, 0x0777 , ja, va ); \
    _mm512_mask_compressstoreu_ps( &pa[ j*Dy ], 0x0777, va); \
  } \
}

#elif ( ORDER == 3 )

#define ACC_CURRENT3D( k, v, p ) { \
  float * const pa = pj + idx[k] + k3 * Dz; \
  \
  for( int j = 0; j < 4; j++) { \
    __m512 va, ja; \
    va = _mm512_mask_expandloadu_ps( va, 0x7777, &pa[ j*Dy ] ); \
    ja = _mm512_permute4f128_ps(                  v[j][0], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[j][1], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0700, v[j][2], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x7000, v[j][3], p ); \
    \
    va = _mm512_mask_add_ps( va, 0x7777 , ja, va ); \
    _mm512_mask_compressstoreu_ps( &pa[ j*Dy ], 0x7777, va); \
  } \
}

#elif ( ORDER == 4 )

#define ACC_CURRENT3D( k, v, p ) { \
  float * const pa = pj + idx[k] + k3 * Dz; \
  \
  for( int j = 0; j < 5; j++) { \
    __m512 va, ja, vb, jb; \
    va = _mm512_mask_expandloadu_ps( va, 0x7777, &pa[ j*Dy ] ); \
    vb = _mm512_loadu_ps( &pa[ j*Dy + 12 ] ); \
    ja = _mm512_permute4f128_ps(                  v[j][0], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[j][1], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x0700, v[j][2], p ); \
    ja = _mm512_mask_permute4f128_ps( ja, 0x7000, v[j][3], p ); \
    jb = _mm512_permute4f128_ps(                  v[j][4], p ); \
    \
    va = _mm512_mask_add_ps( va, 0x7777 , ja, va ); \
    vb = _mm512_mask_add_ps( vb, 0x0007 , jb, vb ); \
    _mm512_mask_compressstoreu_ps( &pa[ j*Dy ], 0x7777, va); \
    _mm512_mask_storeu_ps(    &pa[ j*Dy + 12 ], 0x0007, vb); \
  } \
}

#else
#error Unsupported interpolation order
#endif


void DEP_CURRENT_3D
(float * const  current, int const * const  size, int const * const  offset,
                       float * const norm, t_split_buf3D * const part)
{

  __m512 vx0, vx1, vy0, vy1,  vz0, vz1, vq;

  __m512 vqnx, vqny, vqnz;
  __m512 vs0x[NP], vs1x[NP], vs0y[NP], vs1y[NP], vs0z[NP], vs1z[NP];

  __m512 vwl1[NP], vwl2[NP], vwl3[NP];
  __m512 vwp1[NP][NP], vwp2[NP][NP], vwp3[NP][NP];

  DECLARE_ALIGNED_64( int idx[VEC_WIDTH] );

  int const Dy = 3 * size[0];
  int const Dz = Dy * size[1];

  __m512i const vDy = _mm512_set1_epi32( Dy );
  __m512i const vDz = _mm512_set1_epi32( Dz );

  float* const pj = current + ( offset[0] - OFFSET  ) * 3 +
  ( offset[1] - OFFSET  ) * Dy +
  ( offset[2] - OFFSET  ) * Dz;

  __m512 const vnorm1 = _mm512_set1_ps(norm[0]);
  __m512 const vnorm2 = _mm512_set1_ps(norm[1]);
  __m512 const vnorm3 = _mm512_set1_ps(norm[2]);



  int i;
  int np = part -> np;

  // If number of particles in buffer is not multiple of VEC_WIDTH add dummy particles to the end
  if ( np % VEC_WIDTH ) {
    for( i = 0; i < VEC_WIDTH - np%VEC_WIDTH; i ++ ) {
      int k = np + i;

      part -> x0[k] = part -> x1[k] = 0.;
      part -> y0[k] = part -> y1[k] = 0.;
      part -> z0[k] = part -> z1[k] = 0.;
      part -> q[k] = 0.;
      part -> ix[k] = part -> iy[k] = part -> iz[k] = 1;
    }
  }

  for( i = 0; i < np; i+=VEC_WIDTH ) {

    int k, k1, k2, k3;
    __m512i vix, viy, viz;

    // load VEC_WIDTH particles
    LOAD16P3D( part, i, vx0, vx1, vy0, vy1, vz0, vz1, vq, vix, viy, viz )

    // Store cell index
    vix = _mm512_add_epi32( vix, _mm512_add_epi32( vix, vix ) );
    viy = _mm512_mullo_epi32( viy, vDy );
    viz = _mm512_mullo_epi32( viz, vDz );
    _mm512_store_epi32( (__m512i *) idx, _mm512_add_epi32( _mm512_add_epi32( vix, viy ), viz ) );

    // Get spline weights
    SPLINE( vx0, vs0x );
    SPLINE( vx1, vs1x );
    SPLINE( vy0, vs0y );
    SPLINE( vy1, vs1y );
    SPLINE( vz0, vs0z );
    SPLINE( vz1, vs1z );

    vqnx = _mm512_mul_ps( vq, vnorm1);
    vqny = _mm512_mul_ps( vq, vnorm2);
    vqnz = _mm512_mul_ps( vq, vnorm3);

    // get longitudinal weights
    WL( vqnx, vx0, vx1, vwl1 ); vwl1[NP-1] = _mm512_setzero_ps();
    WL( vqny, vy0, vy1, vwl2 ); vwl2[NP-1] = _mm512_setzero_ps();
    WL( vqnz, vz0, vz1, vwl3 ); vwl3[NP-1] = _mm512_setzero_ps();

    // get perpendicular weights
    for( k2 = 0; k2 < NP; k2++ ) {
      for( k1 = 0; k1 < NP; k1++ ) {

        const __m512 c1_2 = _mm512_set1_ps(0.5f);
        __m512 tmp1, tmp2;

        // wp1[k2][k1] = s0y[k1]*s0z[k2] + s1y[k1]*s1z[k2] +
        //               0.5*( s0y[k1]*s1z[k2] + s1y[k1]*s0z[k2] )

        tmp1 = _mm512_mul_ps( vs0y[k1], vs0z[k2] );
        tmp2 = _mm512_mul_ps( vs0y[k1], vs1z[k2] );
        tmp1 = _mm512_fmadd_ps( vs1y[k1], vs1z[k2], tmp1 );
        tmp2 = _mm512_fmadd_ps( vs1y[k1], vs0z[k2], tmp2 );

        vwp1[k2][k1] = _mm512_fmadd_ps( c1_2, tmp2, tmp1 );

        // wp2[k2][k1] = s0x[k1]*s0z[k2] + s1x[k1]*s1z[k2] +
        //               0.5*( s0x[k1]*s1z[k2] + s1x[k1]*s0z[k2] )

        tmp1 = _mm512_mul_ps( vs0x[k1], vs0z[k2] );
        tmp2 = _mm512_mul_ps( vs0x[k1], vs1z[k2] );
        tmp1 = _mm512_fmadd_ps( vs1x[k1], vs1z[k2], tmp1 );
        tmp2 = _mm512_fmadd_ps( vs1x[k1], vs0z[k2], tmp2 );

        vwp2[k2][k1] = _mm512_fmadd_ps( c1_2, tmp2, tmp1 );

        // wp3[k2][k1] = s0x[k1]*s0y[k2] + s1x[k1]*s1y[k2] +
        //               0.5*( s0x[k1]*s1y[k2] + s1x[k1]*s0y[k2] )

        tmp1 = _mm512_mul_ps( vs0x[k1], vs0y[k2] );
        tmp2 = _mm512_mul_ps( vs0x[k1], vs1y[k2] );
        tmp1 = _mm512_fmadd_ps( vs1x[k1], vs1y[k2], tmp1 );
        tmp2 = _mm512_fmadd_ps( vs1x[k1], vs0y[k2], tmp2 );

        vwp3[k2][k1] = _mm512_fmadd_ps( c1_2, tmp2, tmp1 );

      }
    }

    // Accumulate current 1 plane at a time using the 2D algorithm
    for( k3 = 0; k3 < NP; k3++ ) {
      __m512 a[NP][NP], b[NP][NP], c[NP][NP], d[NP][NP];

      for( k2 = 0; k2 < NP; k2++ ) {

        for ( k1 = 0; k1 < NP; k1++ ) {
          __m512 j1, j2, j3;

          // Calculate current components
          j1 = _mm512_mul_ps( vwl1[k1] , vwp1[k3][k2] );
          j2 = _mm512_mul_ps( vwl2[k2] , vwp2[k3][k1] );
          j3 = _mm512_mul_ps( vwl3[k3] , vwp3[k2][k1] );

          // Do a 16x4 transpose, ignoring 4th current component
          __m512 t0 = _mm512_mask_swizzle_ps( j1, 0xAAAA, j2, _MM_SWIZ_REG_CDAB );
          __m512 t1 = _mm512_mask_swizzle_ps( j3, 0xAAAA, j3, _MM_SWIZ_REG_CDAB );
          __m512 t2 = _mm512_mask_swizzle_ps( j2, 0x5555, j1, _MM_SWIZ_REG_CDAB );
          __m512 t3 = _mm512_mask_swizzle_ps( j3, 0x5555, j3, _MM_SWIZ_REG_CDAB );
          a[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t0), 0xAA, _mm512_castps_pd(t1), _MM_SWIZ_REG_CDAB )));
          b[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t2), 0xAA, _mm512_castps_pd(t3), _MM_SWIZ_REG_CDAB )));
          c[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t1), 0x55, _mm512_castps_pd(t0), _MM_SWIZ_REG_CDAB )));
          d[k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t3), 0x55, _mm512_castps_pd(t2), _MM_SWIZ_REG_CDAB )));
        }
      }

      // Accumulate electric current in k3 plane
      ACC_CURRENT3D(  0, a, _MM_PERM_AAAA )
      ACC_CURRENT3D(  1, b, _MM_PERM_AAAA )
      ACC_CURRENT3D(  2, c, _MM_PERM_AAAA )
      ACC_CURRENT3D(  3, d, _MM_PERM_AAAA )
      ACC_CURRENT3D(  4, a, _MM_PERM_BBBB )
      ACC_CURRENT3D(  5, b, _MM_PERM_BBBB )
      ACC_CURRENT3D(  6, c, _MM_PERM_BBBB )
      ACC_CURRENT3D(  7, d, _MM_PERM_BBBB )
      ACC_CURRENT3D(  8, a, _MM_PERM_CCCC )
      ACC_CURRENT3D(  9, b, _MM_PERM_CCCC )
      ACC_CURRENT3D( 10, c, _MM_PERM_CCCC )
      ACC_CURRENT3D( 11, d, _MM_PERM_CCCC )
      ACC_CURRENT3D( 12, a, _MM_PERM_DDDD )
      ACC_CURRENT3D( 13, b, _MM_PERM_DDDD )
      ACC_CURRENT3D( 14, c, _MM_PERM_DDDD )
      ACC_CURRENT3D( 15, d, _MM_PERM_DDDD )

    }

  }
}


#undef ACC_CURRENT3D


#endif


#if 0

/* mk. II

Pre calculate all currents, deposit 1 particle at a time
This is the fastest version that has the same roundoff as the Fortran version

*/

#warning Using mk. II 3D algorithm

#if ( ORDER == 1 )

#define ACC_CURRENT3D( k, v, p ) { \
  float * const line = pj + idx[k]; \
  for( int k3 = 0; k3 < 2; k3++) { \
    for( int k2 = 0; k2 < 2; k2++) { \
      __m512 va, ja; \
      va = _mm512_mask_expandloadu_ps( va, 0x0077, &line[ k2*Dy + k3*Dz ] ); \
      ja = _mm512_permute4f128_ps(                  v[k3][k2][0], p ); \
      ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[k3][k2][1], p ); \
      \
      va = _mm512_mask_add_ps( va, 0x0077 , ja, va ); \
      _mm512_mask_compressstoreu_ps( &line[ k2*Dy + k3*Dz ], 0x0077, va); \
    } \
  } \
}


#elif ( ORDER == 2 )

#define ACC_CURRENT3D( k, v, p ) { \
  float * const line = pj + idx[k]; \
  for( int k3 = 0; k3 < 3; k3++) { \
    for( int k2 = 0; k2 < 3; k2++) { \
      __m512 va, ja; \
      va = _mm512_mask_expandloadu_ps( va, 0x0777, &line[ k2*Dy + k3*Dz ] ); \
      ja = _mm512_permute4f128_ps(                  v[k3][k2][0], p ); \
      ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[k3][k2][1], p ); \
      ja = _mm512_mask_permute4f128_ps( ja, 0x0700, v[k3][k2][2], p ); \
      \
      va = _mm512_mask_add_ps( va, 0x0777 , ja, va ); \
      _mm512_mask_compressstoreu_ps( &line[ k2*Dy + k3*Dz ], 0x0777, va); \
    } \
  } \
}

#elif ( ORDER == 3 )

#define ACC_CURRENT3D( k, v, p ) { \
  float * const line = pj + idx[k]; \
  for( int k3 = 0; k3 < 4; k3++) { \
    for( int k2 = 0; k2 < 4; k2++) { \
      __m512 va, ja; \
      va = _mm512_mask_expandloadu_ps( va, 0x7777, &line[ k2*Dy + k3*Dz ] ); \
      ja = _mm512_permute4f128_ps(                  v[k3][k2][0], p ); \
      ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[k3][k2][1], p ); \
      ja = _mm512_mask_permute4f128_ps( ja, 0x0700, v[k3][k2][2], p ); \
      ja = _mm512_mask_permute4f128_ps( ja, 0x7000, v[k3][k2][3], p ); \
      \
      va = _mm512_mask_add_ps( va, 0x7777 , ja, va ); \
      _mm512_mask_compressstoreu_ps( &line[ k2*Dy + k3*Dz ], 0x7777, va); \
    } \
  } \
}

#elif ( ORDER == 4 )

#define ACC_CURRENT3D( k, v, p ) { \
  float * const line = pj + idx[k]; \
  for( int k3 = 0; k3 < 5; k3++) { \
    for( int k2 = 0; k2 < 5; k2++) { \
      __m512 va, ja, vb, jb; \
      va = _mm512_mask_expandloadu_ps( va, 0x7777, &line[ k2*Dy + k3*Dz ] ); \
      vb = _mm512_loadu_ps( &line[ k2*Dy + k3*Dz + 12] ); \
      ja = _mm512_permute4f128_ps(                  v[k3][k2][0], p ); \
      ja = _mm512_mask_permute4f128_ps( ja, 0x0070, v[k3][k2][1], p ); \
      ja = _mm512_mask_permute4f128_ps( ja, 0x0700, v[k3][k2][2], p ); \
      ja = _mm512_mask_permute4f128_ps( ja, 0x7000, v[k3][k2][3], p ); \
      jb = _mm512_permute4f128_ps(                  v[k3][k2][4], p ); \
      \
      va = _mm512_mask_add_ps( va, 0x7777 , ja, va ); \
      vb = _mm512_mask_add_ps( vb, 0x0007 , jb, vb ); \
      _mm512_mask_compressstoreu_ps( &line[ k2*Dy + k3*Dz ], 0x7777, va); \
      _mm512_mask_storeu_ps(    &line[ k2*Dy + k3*Dz + 12 ], 0x0007, vb); \
    } \
  } \
}

#else
#error Unsupported interpolation order
#endif


void DEP_CURRENT_3D
(float * const  current, int const * const  size, int const * const  offset,
                       float * const norm, t_split_buf3D * const part)
{

  __m512 vx0, vx1, vy0, vy1,  vz0, vz1, vq;

  __m512 vqnx, vqny, vqnz;
  __m512 vs0x[NP], vs1x[NP], vs0y[NP], vs1y[NP], vs0z[NP], vs1z[NP];

  __m512 vwl1[NP], vwl2[NP], vwl3[NP];
  __m512 vwp1[NP][NP], vwp2[NP][NP], vwp3[NP][NP];

  DECLARE_ALIGNED_64( int idx[VEC_WIDTH] );

  int const Dy = 3 * size[0];
  int const Dz = Dy * size[1];

  __m512i const vDy = _mm512_set1_epi32( Dy );
  __m512i const vDz = _mm512_set1_epi32( Dz );

  float* const pj = current + ( offset[0] - OFFSET  ) * 3 +
  ( offset[1] - OFFSET  ) * Dy +
  ( offset[2] - OFFSET  ) * Dz;

  __m512 const vnorm1 = _mm512_set1_ps(norm[0]);
  __m512 const vnorm2 = _mm512_set1_ps(norm[1]);
  __m512 const vnorm3 = _mm512_set1_ps(norm[2]);



  int i;
  int np = part -> np;

  // If number of particles in buffer is not multiple of VEC_WIDTH add dummy particles to the end
  if ( np % VEC_WIDTH ) {
    for( i = 0; i < VEC_WIDTH - np%VEC_WIDTH; i ++ ) {
      int k = np + i;

      part -> x0[k] = part -> x1[k] = 0.;
      part -> y0[k] = part -> y1[k] = 0.;
      part -> z0[k] = part -> z1[k] = 0.;
      part -> q[k] = 0.;
      part -> ix[k] = part -> iy[k] = part -> iz[k] = 1;
    }
  }

  for( i = 0; i < np; i+=VEC_WIDTH ) {

    int k, k1, k2, k3;
    __m512i vix, viy, viz;

    // load VEC_WIDTH particles
    LOAD16P3D( part, i, vx0, vx1, vy0, vy1, vz0, vz1, vq, vix, viy, viz )

    // Store cell index
    vix = _mm512_add_epi32( vix, _mm512_add_epi32( vix, vix ) );
    viy = _mm512_mullo_epi32( viy, vDy );
    viz = _mm512_mullo_epi32( viz, vDz );
    _mm512_store_epi32( (__m512i *) idx, _mm512_add_epi32( _mm512_add_epi32( vix, viy ), viz ) );

    // Get spline weights
    SPLINE( vx0, vs0x );
    SPLINE( vx1, vs1x );
    SPLINE( vy0, vs0y );
    SPLINE( vy1, vs1y );
    SPLINE( vz0, vs0z );
    SPLINE( vz1, vs1z );

    vqnx = _mm512_mul_ps( vq, vnorm1);
    vqny = _mm512_mul_ps( vq, vnorm2);
    vqnz = _mm512_mul_ps( vq, vnorm3);

    // get longitudinal weights
    WL( vqnx, vx0, vx1, vwl1 ); vwl1[NP-1] = _mm512_setzero_ps();
    WL( vqny, vy0, vy1, vwl2 ); vwl2[NP-1] = _mm512_setzero_ps();
    WL( vqnz, vz0, vz1, vwl3 ); vwl3[NP-1] = _mm512_setzero_ps();

    // get perpendicular weights
    for( k2 = 0; k2 < NP; k2++ ) {
      for( k1 = 0; k1 < NP; k1++ ) {

        const __m512 c1_2 = _mm512_set1_ps(0.5f);
        __m512 tmp1, tmp2;

        // wp1[k2][k1] = s0y[k1]*s0z[k2] + s1y[k1]*s1z[k2] +
        //               0.5*( s0y[k1]*s1z[k2] + s1y[k1]*s0z[k2] )

        tmp1 = _mm512_mul_ps( vs0y[k1], vs0z[k2] );
        tmp2 = _mm512_mul_ps( vs0y[k1], vs1z[k2] );
        tmp1 = _mm512_fmadd_ps( vs1y[k1], vs1z[k2], tmp1 );
        tmp2 = _mm512_fmadd_ps( vs1y[k1], vs0z[k2], tmp2 );

        vwp1[k2][k1] = _mm512_fmadd_ps( c1_2, tmp2, tmp1 );

        // wp2[k2][k1] = s0x[k1]*s0z[k2] + s1x[k1]*s1z[k2] +
        //               0.5*( s0x[k1]*s1z[k2] + s1x[k1]*s0z[k2] )

        tmp1 = _mm512_mul_ps( vs0x[k1], vs0z[k2] );
        tmp2 = _mm512_mul_ps( vs0x[k1], vs1z[k2] );
        tmp1 = _mm512_fmadd_ps( vs1x[k1], vs1z[k2], tmp1 );
        tmp2 = _mm512_fmadd_ps( vs1x[k1], vs0z[k2], tmp2 );

        vwp2[k2][k1] = _mm512_fmadd_ps( c1_2, tmp2, tmp1 );

        // wp3[k2][k1] = s0x[k1]*s0y[k2] + s1x[k1]*s1y[k2] +
        //               0.5*( s0x[k1]*s1y[k2] + s1x[k1]*s0y[k2] )

        tmp1 = _mm512_mul_ps( vs0x[k1], vs0y[k2] );
        tmp2 = _mm512_mul_ps( vs0x[k1], vs1y[k2] );
        tmp1 = _mm512_fmadd_ps( vs1x[k1], vs1y[k2], tmp1 );
        tmp2 = _mm512_fmadd_ps( vs1x[k1], vs0y[k2], tmp2 );

        vwp3[k2][k1] = _mm512_fmadd_ps( c1_2, tmp2, tmp1 );

      }
    }

    // Calculate currents for all particles
    __m512 a[NP][NP][NP], b[NP][NP][NP], c[NP][NP][NP], d[NP][NP][NP];
    for( k3 = 0; k3 < NP; k3++ ) {

      for( k2 = 0; k2 < NP; k2++ ) {

        for ( k1 = 0; k1 < NP; k1++ ) {
          __m512 j1, j2, j3;

          // Calculate current components
          j1 = _mm512_mul_ps( vwl1[k1] , vwp1[k3][k2] );
          j2 = _mm512_mul_ps( vwl2[k2] , vwp2[k3][k1] );
          j3 = _mm512_mul_ps( vwl3[k3] , vwp3[k2][k1] );

          // Do a 16x4 transpose, ignoring 4th current component
          __m512 t0 = _mm512_mask_swizzle_ps( j1, 0xAAAA, j2, _MM_SWIZ_REG_CDAB );
          __m512 t1 = _mm512_mask_swizzle_ps( j3, 0xAAAA, j3, _MM_SWIZ_REG_CDAB );
          __m512 t2 = _mm512_mask_swizzle_ps( j2, 0x5555, j1, _MM_SWIZ_REG_CDAB );
          __m512 t3 = _mm512_mask_swizzle_ps( j3, 0x5555, j3, _MM_SWIZ_REG_CDAB );
          a[k3][k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t0), 0xAA, _mm512_castps_pd(t1), _MM_SWIZ_REG_CDAB )));
          b[k3][k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t2), 0xAA, _mm512_castps_pd(t3), _MM_SWIZ_REG_CDAB )));
          c[k3][k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t1), 0x55, _mm512_castps_pd(t0), _MM_SWIZ_REG_CDAB )));
          d[k3][k2][k1] = _mm512_castpd_ps((_mm512_mask_swizzle_pd( _mm512_castps_pd(t3), 0x55, _mm512_castps_pd(t2), _MM_SWIZ_REG_CDAB )));
        }
      }
    }

    // Accumulate electric current volume loopeing over particle
    ACC_CURRENT3D(  0, a, _MM_PERM_AAAA )
    ACC_CURRENT3D(  1, b, _MM_PERM_AAAA )
    ACC_CURRENT3D(  2, c, _MM_PERM_AAAA )
    ACC_CURRENT3D(  3, d, _MM_PERM_AAAA )
    ACC_CURRENT3D(  4, a, _MM_PERM_BBBB )
    ACC_CURRENT3D(  5, b, _MM_PERM_BBBB )
    ACC_CURRENT3D(  6, c, _MM_PERM_BBBB )
    ACC_CURRENT3D(  7, d, _MM_PERM_BBBB )
    ACC_CURRENT3D(  8, a, _MM_PERM_CCCC )
    ACC_CURRENT3D(  9, b, _MM_PERM_CCCC )
    ACC_CURRENT3D( 10, c, _MM_PERM_CCCC )
    ACC_CURRENT3D( 11, d, _MM_PERM_CCCC )
    ACC_CURRENT3D( 12, a, _MM_PERM_DDDD )
    ACC_CURRENT3D( 13, b, _MM_PERM_DDDD )
    ACC_CURRENT3D( 14, c, _MM_PERM_DDDD )
    ACC_CURRENT3D( 15, d, _MM_PERM_DDDD )


  }
}


#undef ACC_CURRENT3D


#endif


#undef DEP_CURRENT_1D
#undef DEP_CURRENT_2D
#undef DEP_CURRENT_3D
#undef SPLINE
#undef WL

#undef OFFSET
#undef ORDER
#undef NP

#endif







