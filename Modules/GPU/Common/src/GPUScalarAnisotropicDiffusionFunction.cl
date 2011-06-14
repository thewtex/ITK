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
#define BUFPIXELTYPE float
//
// Dim 1
//
#ifdef DIM_1
#define ImageDimension 1
__kernel void AverageGradientMagnitudeSquared(__global const INPIXELTYPE *in, __global BUFPIXELTYPE *buf, float scalex, int width)
{
	int gix = get_global_id(0);

	// NOTE! 1D version is not implemented
	if(gix < width)
  {
		buf[gix] = 0;
  }
}
#endif

//
// Dim 2
//
#ifdef DIM_2
#define ImageDimension 2
__kernel void AverageGradientMagnitudeSquared(__global const INPIXELTYPE *in, __global BUFPIXELTYPE *buf, float scalex, float scaley, int width, int height)
{
	// global index
  int gix = get_global_id(0);
  int giy = get_global_id(1);
  unsigned int gidx = width*giy + gix;

  // local index for shared memory.. note 1-pixel boundary for shared memory
  int lix = get_local_id(0) + 1;
  int liy = get_local_id(1) + 1;

  // centralized derivatives, half directonal derivatives
  float dx[2];

  __local float sm[BLOCK_SIZE+2][BLOCK_SIZE+2];

  // Read (BLOCK_SIZE+2)x(BLOCK_SIZE+2) data - 1 pixel boundary around block

  // inner
  if(gix < width && giy < height)
  {
    sm[lix][liy] = in[gidx];
  }
  else sm[lix][liy] = 0;

  barrier(CLK_LOCAL_MEM_FENCE);

  // boundary
  if(gix < width && giy < height)
  {
    if(lix == 1)
    {
      // Left
      if(gix > 0) sm[lix-1][liy] = in[gidx-1];
      else sm[lix-1][liy] = in[gidx];
    }
    else if(lix == BLOCK_SIZE || gix == width-1)
    {
      // Right
      if(gix < (width-1)) sm[lix+1][liy] = in[gidx+1];
      else sm[lix+1][liy] = in[gidx];
    }
    else {}

    if(liy == 1)
    {
      // Bottom
      if(giy > 0) sm[lix][liy-1] = in[gidx - width];
      else sm[lix][liy-1] = in[gidx];
    }
    else if(liy == BLOCK_SIZE || giy == height-1)
    {
      // Up
      if(giy < (height-1)) sm[lix][liy+1] = in[gidx + width];
      else sm[lix][liy+1] = in[gidx];
    }
    else {}
  }

  // Synchronize shared memory
  barrier(CLK_LOCAL_MEM_FENCE);

  float val = 0;

  // Compute Update
  if(gix < width && giy < height)
  {
    // centralized derivatives
    dx[0] = (sm[lix+1][liy] - sm[lix-1][liy])*0.5f*scalex;  // grad x
    dx[1] = (sm[lix][liy+1] - sm[lix][liy-1])*0.5f*scaley;  // grad y

		//sm[lix][liy] = dx[0]*dx[0] + dx[1]*dx[1];
    val = dx[0]*dx[0] + dx[1]*dx[1];
  }

  // Synchronize shared memory
  barrier(CLK_LOCAL_MEM_FENCE);

  sm[lix][liy] = val;

  barrier(CLK_LOCAL_MEM_FENCE);

  // Reduction on shared memory

  // Reducton along y
  int interval = BLOCK_SIZE/2;

  while(interval > 0)
  {
    if(liy <= interval)
    {
      sm[lix][liy] += sm[lix][liy + interval];
    }

    interval = interval >> 1; // divide by 2

    barrier(CLK_LOCAL_MEM_FENCE);
  }

  // Reduction along x
  if(liy == 1)
  {
    interval = BLOCK_SIZE/2;

    while(interval > 0)
    {
      if(lix <= interval)
      {
        sm[lix][liy] += sm[lix + interval][liy];
      }

      //if(interval > 16) barrier(CLK_LOCAL_MEM_FENCE);  // don't need to synchronize if within a warp (only for NVIDIA)

      barrier(CLK_LOCAL_MEM_FENCE);

      interval = interval >> 1; // divide by 2
    }

    // write the final value to global memory
    if(lix == 1)
    {
      // Compute group index
      gix = get_group_id(0);
      giy = get_group_id(1);
      gidx = get_num_groups(0)*giy + gix;

      // Write the final value to global memory
      buf[gidx] = sm[lix][liy];
    }
  }

}
#endif

//
// Dim 3
//
#ifdef DIM_3
#define ImageDimension 3
__kernel void AverageGradientMagnitudeSquared(__global const INPIXELTYPE *in, __global BUFPIXELTYPE *buf, float scalex, float scaley, float scalez, int width, int height, int depth)
{
	int gix = get_global_id(0);
  int giy = get_global_id(1);
	int giz = get_global_id(2);
	unsigned int gidx = (unsigned int)width*((unsigned int)(giz*height + giy)) + (unsigned int)gix;

  // local index for shared memory.. note 1-pixel boundary for shared memory
  int lix = get_local_id(0);
  int liy = get_local_id(1);
  int liz = get_local_id(2);

  // centralized derivatives, half directonal derivatives
  float dx[3];

  // shared memory - split boundary and center to avoid bank conflict
  __local float sm[BLOCK_SIZE][BLOCK_SIZE][BLOCK_SIZE];
  __local float yz_l[BLOCK_SIZE][BLOCK_SIZE]; // for x boundary
  __local float yz_r[BLOCK_SIZE][BLOCK_SIZE];
  __local float xz_l[BLOCK_SIZE][BLOCK_SIZE]; // for y boundary
  __local float xz_r[BLOCK_SIZE][BLOCK_SIZE];
  __local float xy_l[BLOCK_SIZE][BLOCK_SIZE]; // for z boundary
  __local float xy_r[BLOCK_SIZE][BLOCK_SIZE];

  //
  // Read (BLOCK_SIZE+2)x(BLOCK_SIZE+2) data - 1 pixel boundary around block
  //

  // Center
  if(gix < width && giy < height && giz < depth)
  {
    sm[lix][liy][liz] = in[gidx];
  }
  else sm[lix][liy][liz] = 0;

  // 6 top/bottom/left/right/up/down neighbor planes
  if(gix < width && giy < height && giz < depth)
  {
    if(lix == 0)
    {
      // y/z top plane
      if(gix > 0) yz_l[liy][liz] = in[gidx-1];
      else yz_l[liy][liz] = in[gidx];
    }
    else if(lix == BLOCK_SIZE-1 || gix == width-1)
    {
      // y/z bottom plane
      if(gix < (width-1)) yz_r[liy][liz] = in[gidx+1];
      else yz_r[liy][liz] = in[gidx];
    }
    else {}

    if(liy == 0)
    {
      // x/z top plane
      if(giy > 0) xz_l[lix][liz] = in[gidx - width];
      else xz_l[lix][liz] = in[gidx];
    }
    else if(liy == BLOCK_SIZE-1 || giy == height-1)
    {
      // x/z bottom plane
      if(giy < (height-1)) xz_r[lix][liz] = in[gidx + width];
      else xz_r[lix][liz] = in[gidx];
    }
    else {}

    if(liz == 0)
    {
      // x/y top plane
      if(giz > 0) xy_l[lix][liy] = in[gidx - width*height];
      else xy_l[lix][liy] = in[gidx];
    }
    else if(liz == BLOCK_SIZE-1 || giz == depth-1)
    {
      // x/z bottom plane
      if(giz < (depth-1)) xy_r[lix][liy] = in[gidx + width*height];
      else xy_r[lix][liy] = in[gidx];
    }
    else {}
  }

  // synchronize shared memory
  barrier(CLK_LOCAL_MEM_FENCE);

  float val = 0;

  // Compute Update
  if(gix < width && giy < height && giz < depth)
  {
    // centralized derivatives
    float df, db;

    df = (lix == BLOCK_SIZE-1 || gix == width-1) ? yz_r[liy][liz] : sm[lix+1][liy][liz];
    db = (lix == 0) ? yz_l[liy][liz] : sm[lix-1][liy][liz];
    dx[0] = (df - db)*0.5f*scalex;

    df = (liy == BLOCK_SIZE-1 || giy == height-1) ? xz_r[lix][liz] : sm[lix][liy+1][liz];
    db = (liy == 0) ? xz_l[lix][liz] : sm[lix][liy-1][liz];
    dx[1] = (df - db)*0.5f*scaley;

    df = (liz == BLOCK_SIZE-1 || giz == depth-1) ? xy_r[lix][liy] : sm[lix][liy][liz+1];
    db = (liz == 0) ? xy_l[lix][liy] : sm[lix][liy][liz-1];
    dx[2] = (df - db)*0.5f*scalez;

		val = dx[0]*dx[0] + dx[1]*dx[1] * dx[2]*dx[2];
  }

  barrier(CLK_LOCAL_MEM_FENCE);

  sm[lix][liy][liz] = val;

  barrier(CLK_LOCAL_MEM_FENCE);

  //
  // Reduction by Sum
  //
  int interval = BLOCK_SIZE/2;

  while(interval > 0) // reduction along z axis
  {
    if(liz < interval)
    {
      sm[lix][liy][liz] += sm[lix][liy][liz + interval];
    }

    interval = interval >> 1; // divide by 2

    barrier(CLK_LOCAL_MEM_FENCE);
  }

  if(liz == 0) // x-y plane
  {
    interval = BLOCK_SIZE/2;

    while(interval > 0)
    {
      if(liy < interval)
      {
        sm[lix][liy][liz] += sm[lix][liy + interval][liz];
      }

      interval = interval >> 1; // divide by 2

      barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Reduction along x
    if(liy == 0)
    {
      interval = BLOCK_SIZE/2;

      while(interval > 0)
      {
        if(lix < interval)
        {
          sm[lix][liy][liz] += sm[lix + interval][liy][liz];
        }

        //if(interval > 16) barrier(CLK_LOCAL_MEM_FENCE);  // don't need to synchronize if within a warp (only for NVIDIA)

        barrier(CLK_LOCAL_MEM_FENCE);

        interval = interval >> 1; // divide by 2
      }

      // write the final value to global memory
      if(lix == 0)
      {
        // Compute group index
        gix = get_group_id(0);
        giy = get_group_id(1);
        giz = get_group_id(2);

        gidx = get_num_groups(0)*(get_num_groups(1)*giz + giy) + gix;

        // Write the final value to global memory
        buf[gidx] = sm[lix][liy][liz];
      }
    }
  }
}
#endif
