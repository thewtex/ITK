/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

// assume input and output pixel type is same
#define INPIXELTYPE PIXELTYPE
#define BUFPIXELTYPE PIXELTYPE

__constant unsigned int m_Stride[3] = {1, 3, 9};

// forward declaration
BUFPIXELTYPE Functor(__global const INPIXELTYPE *, unsigned int, float *, float);

#ifdef DIM_1
#define ImageDimension 1
__kernel void ComputeUpdate(__global const INPIXELTYPE *in, __global BUFPIXELTYPE *buf, int width)
{
	int gix = get_global_id(0);

	// test
	if(gix < width)
  {
		buf[gix] = (BUFPIXELTYPE)( gix%255 );
  }
}
#endif

#ifdef DIM_2
#define ImageDimension 2
__kernel void ComputeUpdate(__global const INPIXELTYPE *in, __global BUFPIXELTYPE *buf, PIXELTYPE K, float scalex, float scaley, int width, int height)
{
	int gix = get_global_id(0);
  int giy = get_global_id(1);
  unsigned int gidx = width*giy + gix;

  __local float shrm[BLOCK_SIZE][BLOCK_SIZE];

  float scale[2];
  scale[0] = scalex;
  scale[1] = scaley;

	// test
	if(gix < width && giy < height)
  {
		buf[gix] = Functor( in, gix, scale, K );
  }
}
#endif

#ifdef DIM_3
#define ImageDimension 3
__kernel void ComputeUpdate(__global const INPIXELTYPE *in, __global BUFPIXELTYPE *buf, int width, int height, int depth)
{
	int gix = get_global_id(0);
    int giy = get_global_id(1);
    unsigned int gidx = width*giy + gix;
	int giz = get_global_id(2);
	unsigned int gidx = width*(giz*heigh + giy) + gix;
	if(gix < width && giy < height && giz < depth)
	{
		buf[gix] = (BUFPIXELTYPE)( gix%255 );
  }
}
#endif

BUFPIXELTYPE Functor(__global const INPIXELTYPE *it, unsigned int idx, float m_ScaleCoefficients[], PIXELTYPE m_K)
{
  unsigned int i, j;

  float accum;
  float accum_d;
  float Cx;
  float Cxd;

  // PixelType is scalar in this context
  float delta;
  float dx_forward;
  float dx_backward;
  float dx[ImageDimension];
  float dx_aug;
  float dx_dim;

  delta = 0.0f;

  // Calculate the centralized derivatives for each dimension.
  for ( i = 0; i < ImageDimension; i++ )
    {
    dx[i]  =  ( (float)(it[idx + m_Stride[i]]) - (float)(it[idx - m_Stride[i]]) ) / 2.0f;
    dx[i] *= m_ScaleCoefficients[i];
    }

  for ( i = 0; i < ImageDimension; i++ )
    {
    // ``Half'' directional derivatives
    dx_forward = (float)(it[idx + m_Stride[i]]) - (float)(it[idx]);
    dx_forward *= m_ScaleCoefficients[i];
    dx_backward = (float)(it[idx]) - (float)(it[idx - m_Stride[i]]);
    dx_backward *= m_ScaleCoefficients[i];

    // Calculate the conductance terms.  Conductance varies with each
    // dimension because the gradient magnitude approximation is different
    // along each  dimension.
    accum   = 0.0f;
    accum_d = 0.0f;
    for ( j = 0; j < ImageDimension; j++ )
      {
      if ( j != i )
        {
        dx_aug = ( (float)(it[idx + m_Stride[i] + m_Stride[j]]) - (float)(it[idx + m_Stride[i] - m_Stride[j]]) ) / 2.0f;
        dx_aug *= m_ScaleCoefficients[j];
        dx_dim = ( (float)(it[idx - m_Stride[i] + m_Stride[j]]) - (float)(it[idx - m_Stride[i] - m_Stride[j]]) ) / 2.0f;
        dx_dim *= m_ScaleCoefficients[j];
        accum += 0.25f * sqrt(dx[j] + dx_aug);
        accum_d += 0.25f * sqrt(dx[j] + dx_dim);
        }
      }

    if ( m_K > 0 || m_K < 0 )
      {
      Cx  = exp( ( sqrt(dx_forward) + accum )  / m_K );
      Cxd = exp( ( sqrt(dx_backward) + accum_d ) / m_K );
      }
	  else
	  {
	  Cx = 0.0;
      Cxd = 0.0;
	  }

    // Conductance modified first order derivatives.
    dx_forward  = dx_forward * Cx;
    dx_backward = dx_backward * Cxd;

    // Conductance modified second order derivative.
    delta += dx_forward - dx_backward;
    }

  return (BUFPIXELTYPE)( delta );
}
