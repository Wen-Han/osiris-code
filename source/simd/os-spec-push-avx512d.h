#ifndef _OS_SPEC_PUSH_H_AVX512D
#define _OS_SPEC_PUSH_H_AVX512D

/* Size of particle buffer */
/* These must be a multiple of 8 */
#define p_cache_size_1D 64
#define p_cache_size_2D 464
#define p_cache_size_3D 160

#include "vector-avx512.h"

/*****************************************************************************************
t_split_buf1D

Buffer to hold virtual particles for current deposition in 1D.
*****************************************************************************************/

typedef struct {
  // 2 splits maximum
  DECLARE_ALIGNED_64( double x0[ 2 * p_cache_size_1D ] );
  DECLARE_ALIGNED_64( double x1[ 2 * p_cache_size_1D ] );
  DECLARE_ALIGNED_64( double  q[ 2 * p_cache_size_1D ] );
  DECLARE_ALIGNED_64( double vy[ 2 * p_cache_size_1D ] );
  DECLARE_ALIGNED_64( double vz[ 2 * p_cache_size_1D ] );

  DECLARE_ALIGNED_64( int   ix[ 2 * p_cache_size_1D ] );

  unsigned int np;
} t_split_buf1D;


// Because of particle splitting the stores are not always aligned
#define STOREU8P1D( buf, idx, vx0, vx1, vq, vvy, vvz, vix ) { \
  _mm512_storeu_pd( &buf.x0[ idx ], vx0); \
  _mm512_storeu_pd( &buf.x1[ idx ], vx1); \
  _mm512_storeu_pd(  &buf.q[ idx ],  vq); \
  _mm512_storeu_pd( &buf.vy[ idx ], vvy); \
  _mm512_storeu_pd( &buf.vz[ idx ], vvz); \
\
   _mm256_storeu_si256( (__m256i *) &buf.ix[ idx ], vix); \
}

// For the current deposition routines the loads will always be aligned

#define LOAD8P1D( pbuf, idx, vx0, vx1, vq, vvy, vvz, vix ) { \
   vx0 = _mm512_load_pd( &(pbuf->x0[ idx ]) ); \
   vx1 = _mm512_load_pd( &(pbuf->x1[ idx ]) ); \
   vq  = _mm512_load_pd( &(pbuf->q[ idx ]) ); \
   vvy = _mm512_load_pd( &(pbuf->vy[ idx ]) ); \
   vvz = _mm512_load_pd( &(pbuf->vz[ idx ]) ); \
\
   vix = _mm256_load_si256( (const __m256i *) &(pbuf->ix[ idx ]) ); \
}


/*****************************************************************************************
t_split_buf2D

Buffer to hold virtual particles for current deposition in 2D.
*****************************************************************************************/

typedef struct {
  // 3 splits maximum
  DECLARE_ALIGNED_64( double x0[ 3 * p_cache_size_2D ] );
  DECLARE_ALIGNED_64( double x1[ 3 * p_cache_size_2D ] );
  DECLARE_ALIGNED_64( double y0[ 3 * p_cache_size_2D ] );
  DECLARE_ALIGNED_64( double y1[ 3 * p_cache_size_2D ] );
  DECLARE_ALIGNED_64( double  q[ 3 * p_cache_size_2D ] );
  DECLARE_ALIGNED_64( double vz[ 3 * p_cache_size_2D ] );

  DECLARE_ALIGNED_64( int   ix[ 3 * p_cache_size_2D ] );
  DECLARE_ALIGNED_64( int   iy[ 3 * p_cache_size_2D ] );

  unsigned int np;
} t_split_buf2D;


// Because of particle splitting the stores are not always aligned
#define STOREU8P2D( buf, idx, vx0, vx1, vy0, vy1, vq, vvz, vix, viy ) { \
   _mm512_storeu_pd( &buf.x0[ idx ], vx0); \
   _mm512_storeu_pd( &buf.x1[ idx ], vx1); \
   _mm512_storeu_pd( &buf.y0[ idx ], vy0); \
   _mm512_storeu_pd( &buf.y1[ idx ], vy1); \
   _mm512_storeu_pd(  &buf.q[ idx ],  vq); \
   _mm512_storeu_pd( &buf.vz[ idx ], vvz); \
\
   _mm256_storeu_si256( (__m256i *) &buf.ix[ idx ], vix); \
   _mm256_storeu_si256( (__m256i *) &buf.iy[ idx ], viy); \
}

// For the current deposition routines the loads will always be aligned

#define LOAD8P2D( pbuf, idx, vx0, vx1, vy0, vy1, vq, vvz, vix, viy ) { \
   vx0 = _mm512_load_pd( &(pbuf->x0[ idx ]) ); \
   vx1 = _mm512_load_pd( &(pbuf->x1[ idx ]) ); \
   vy0 = _mm512_load_pd( &(pbuf->y0[ idx ]) ); \
   vy1 = _mm512_load_pd( &(pbuf->y1[ idx ]) ); \
   vq  = _mm512_load_pd( &(pbuf->q[ idx ]) ); \
   vvz = _mm512_load_pd( &(pbuf->vz[ idx ]) ); \
\
   vix = _mm256_load_si256( (const __m256i *) &(pbuf->ix[ idx ]) ); \
   viy = _mm256_load_si256( (const __m256i *) &(pbuf->iy[ idx ]) ); \
}

/*****************************************************************************************
t_split_buf3D

Buffer to hold virtual particles for current deposition in 3D.
*****************************************************************************************/

typedef struct {
  // 4 splits maximum
  DECLARE_ALIGNED_64( double x0[ 4 * p_cache_size_3D ] );
  DECLARE_ALIGNED_64( double x1[ 4 * p_cache_size_3D ] );
  DECLARE_ALIGNED_64( double y0[ 4 * p_cache_size_3D ] );
  DECLARE_ALIGNED_64( double y1[ 4 * p_cache_size_3D ] );
  DECLARE_ALIGNED_64( double z0[ 4 * p_cache_size_3D ] );
  DECLARE_ALIGNED_64( double z1[ 4 * p_cache_size_3D ] );
  DECLARE_ALIGNED_64( double  q[ 4 * p_cache_size_3D ] );

  DECLARE_ALIGNED_64( int   ix[ 4 * p_cache_size_3D ] );
  DECLARE_ALIGNED_64( int   iy[ 4 * p_cache_size_3D ] );
  DECLARE_ALIGNED_64( int   iz[ 4 * p_cache_size_3D ] );

  unsigned int np;
} t_split_buf3D;



// Because of particle splitting the stores are not always aligned



// Reference

#define STOREU8P3D( buf, idx, vx0, vx1, vy0, vy1, vz0, vz1, vq, vix, viy, viz ) { \
   _mm512_storeu_pd( &buf.x0[ idx ], vx0); \
   _mm512_storeu_pd( &buf.x1[ idx ], vx1); \
   _mm512_storeu_pd( &buf.y0[ idx ], vy0); \
   _mm512_storeu_pd( &buf.y1[ idx ], vy1); \
   _mm512_storeu_pd( &buf.z0[ idx ], vz0); \
   _mm512_storeu_pd( &buf.z1[ idx ], vz1); \
   _mm512_storeu_pd(  &buf.q[ idx ],  vq); \
   _mm256_storeu_si256( (__m256i *) &buf.ix[ idx ], vix); \
   _mm256_storeu_si256( (__m256i *) &buf.iy[ idx ], viy); \
   _mm256_storeu_si256( (__m256i *) &buf.iz[ idx ], viz); \
}

// For the current deposition routines the loads will always be aligned

#define LOAD8P3D( pbuf, idx, vx0, vx1, vy0, vy1, vz0, vz1, vq, vix, viy, viz ) { \
   vx0 = _mm512_load_pd( &(pbuf->x0[ idx ]) ); \
   vx1 = _mm512_load_pd( &(pbuf->x1[ idx ]) ); \
   vy0 = _mm512_load_pd( &(pbuf->y0[ idx ]) ); \
   vy1 = _mm512_load_pd( &(pbuf->y1[ idx ]) ); \
   vz0 = _mm512_load_pd( &(pbuf->z0[ idx ]) ); \
   vz1 = _mm512_load_pd( &(pbuf->z1[ idx ]) ); \
   vq  = _mm512_load_pd( &(pbuf->q[ idx ]) ); \
\
   vix = _mm256_load_si256( (const __m256i *) &(pbuf->ix[ idx ]) ); \
   viy = _mm256_load_si256( (const __m256i *) &(pbuf->iy[ idx ]) ); \
   viz = _mm256_load_si256( (const __m256i *) &(pbuf->iz[ idx ]) ); \
}

#endif
