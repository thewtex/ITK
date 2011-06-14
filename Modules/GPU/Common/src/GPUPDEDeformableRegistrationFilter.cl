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

//
// Apply Update : out = out + dt*buf
//

#ifdef DIM_1
#define DIM 1
__kernel void SmoothDeformationField(__global const OUTPIXELTYPE *out,
                          __global OUTPIXELTYPE *swap,
						              __global OUTPIXELTYPE *kernel, int ksize,
                          int width)
{
  int gix = get_global_id(0);
  OUTPIXELTYPE dotx;
  int i, ext;
  int radius = ksize / 2;

  if(gix < width)
  {
  dotx = 0;
  for (i=0; i<ksize; i++)
    {
    ext = gix + i - radius;
    if (ext < 0 || ext >= width) continue;
    dotx += out[gix+(i-radius)] * kernel[i];
    }
  swap[gix] = dotx;
  }
}
#endif

#ifdef DIM_2
#define DIM 2
__kernel void SmoothDeformationField(__global const OUTPIXELTYPE *out,
                          __global OUTPIXELTYPE *swap,
						              __global OUTPIXELTYPE *kernel, int ksize,
                          int width, int height)
{
  int gix = get_global_id(0);
  int giy = get_global_id(1);

  OUTPIXELTYPE dotx, doty;
  int i, ext;
  int radius = ksize / 2;

  unsigned int gidx = DIM * (width*giy + gix);

  if(gix < width && giy < height)
  {
  //smoothing along x direction
  dotx = 0;
  doty = 0;
  for (i=0; i<ksize; i++)
    {
    ext = gix + i - radius;
    if (ext < 0 || ext >= width) continue;
    dotx += out[gidx+(i-radius)*DIM  ] * kernel[i];
    doty += out[gidx+(i-radius)*DIM+1] * kernel[i];
    }
  swap[gidx]   = dotx;
  swap[gidx+1] = doty;

  //wait for x direction to be done
  barrier(CL_GLOBAL_MEM_FENCE);

  //smoothing along y direction
  dotx = 0;
  doty = 0;
  for (i=0; i<ksize; i++)
    {
    ext = giy + i - radius;
    if (ext < 0 || ext >= height) continue;
    dotx += swap[gidx+(i-radius)*width*DIM  ] * kernel[i];
    doty += swap[gidx+(i-radius)*width*DIM+1] * kernel[i];
    }
  out[gidx]   = dotx;
  out[gidx+1] = doty;

  }
}
#endif

#ifdef DIM_3
#define DIM 3
__kernel void SmoothDeformationField(__global const OUTPIXELTYPE *out,
                          __global OUTPIXELTYPE *swap,
						              __global OUTPIXELTYPE *kernel, int ksize,
                          int width, int height, int depth)
{
  int gix = get_global_id(0);
  int giy = get_global_id(1);
  int giz = get_global_id(2);

  OUTPIXELTYPE dotx, doty, dotz;
  int i, ext;
  int radius = ksize / 2;

  unsigned int gidx = DIM*(width*(giz*height + giy) + gix);

  if(gix < width && giy < height && giz < depth)
  {
  //smoothing along x direction
  dotx = 0;
  doty = 0;
  dotz = 0;
  for (i=0; i<ksize; i++)
    {
    ext = gix + i - radius;
    if (ext < 0 || ext >= width) continue;
    dotx += out[gidx+(i-radius)*DIM  ] * kernel[i];
    doty += out[gidx+(i-radius)*DIM+1] * kernel[i];
    dotz += out[gidx+(i-radius)*DIM+2] * kernel[i];
    }
  swap[gidx]   = dotx;
  swap[gidx+1] = doty;
  swap[gidx+2] = dotz;

  //wait for x direction to be done
  barrier(CL_GLOBAL_MEM_FENCE);

  //smoothing along y direction
  dotx = 0;
  doty = 0;
  dotz = 0;
  for (i=0; i<ksize; i++)
    {
    ext = giy + i - radius;
    if (ext < 0 || ext >= height) continue;
    dotx += swap[gidx+(i-radius)*width*DIM  ] * kernel[i];
    doty += swap[gidx+(i-radius)*width*DIM+1] * kernel[i];
    dotz += swap[gidx+(i-radius)*width*DIM+2] * kernel[i];
    }
  out[gidx]   = dotx;
  out[gidx+1] = doty;
  out[gidx+2] = dotz;

  //wait for y direction to be done
  barrier(CL_GLOBAL_MEM_FENCE);

  //smoothing along y direction
  dotx = 0;
  doty = 0;
  dotz = 0;
  for (i=0; i<ksize; i++)
    {
    ext = giz + i - radius;
    if (ext < 0 || ext >= depth) continue;
    dotx += out[gidx+(i-radius)*width*height*DIM  ] * kernel[i];
    doty += out[gidx+(i-radius)*width*height*DIM+1] * kernel[i];
    dotz += out[gidx+(i-radius)*width*height*DIM+2] * kernel[i];
    }
  swap[gidx]   = dotx;
  swap[gidx+1] = doty;
  swap[gidx+2] = dotz;

  //wait for z direction to be done
  barrier(CL_GLOBAL_MEM_FENCE);

  out[gidx]   = swap[gidx];
  out[gidx+1] = swap[gidx+1];
  out[gidx+2] = swap[gidx+2];
  }
}
#endif
