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

__kernel void SmoothDeformationField(__global OUTPIXELTYPE *out,
                          __global OUTPIXELTYPE *swap,
						              __constant OUTPIXELTYPE *filter, int ksize,
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
    dotx += out[gix+(i-radius)] * filter[i];
    }
  swap[gix] = dotx;

  //wait for x direction to be done
  barrier(CLK_GLOBAL_MEM_FENCE);

  out[gix] = swap[gix];
  }
}
#endif

#ifdef DIM_2
#define DIM 2

__kernel void SmoothDeformationField(__global OUTPIXELTYPE *out,
                          __global OUTPIXELTYPE *swap,
						              __constant OUTPIXELTYPE *filter, int ksize,
                          int width, int height)
{
  int gix = get_global_id(0);
  int giy = get_global_id(1);

  OUTPIXELTYPE dotx, doty;
  int i, ext;
  int radius = ksize / 2;

  unsigned int gidx = DIM * (width*giy + gix);

  /*if (gix < ksize) {
    out[gidx*DIM] = ksize;
    out[gidx*DIM+1] = width;
    out[gidx*DIM+2] = filter[0];
  }
  return;*/

  if(gix < width && giy < height)
  {
  //smoothing along x direction
  dotx = 0;
  doty = 0;
  for (i=0; i<ksize; i++)
    {
    ext = gix + i - radius;
    if (ext < 0 || ext >= width) continue;
    dotx += out[gidx+(i-radius)*DIM  ] * filter[i];
    doty += out[gidx+(i-radius)*DIM+1] * filter[i];
    }
  swap[gidx]   = dotx;
  swap[gidx+1] = doty;

  //wait for x direction to be done
  barrier(CLK_GLOBAL_MEM_FENCE);

  //smoothing along y direction
  dotx = 0;
  doty = 0;
  for (i=0; i<ksize; i++)
    {
    ext = giy + i - radius;
    if (ext < 0 || ext >= height) continue;
    dotx += swap[gidx+(i-radius)*width*DIM  ] * filter[i];
    doty += swap[gidx+(i-radius)*width*DIM+1] * filter[i];
    }
  out[gidx]   = dotx;
  out[gidx+1] = doty;

  }
}
#endif

#ifdef DIM_3
#define DIM 3
__kernel void SmoothDeformationField(__global OUTPIXELTYPE *out,
                          __global OUTPIXELTYPE *swap,
						              __constant OUTPIXELTYPE *filter, int ksize,
                          int width, int height, int depth)
{
  int gix = get_global_id(0);
  int giy = get_global_id(1);
  int giz = get_global_id(2);

  OUTPIXELTYPE dotx, doty, dotz;
  int i, ext;
  int radius = ksize / 2;

  unsigned int gidx = DIM*(width*(giz*height + giy) + gix);

  /* NOTE: More than three-level nested conditional statements (e.g.,
     if A && B && C..) invalidates command queue during kernel
     execution on Apple OpenCL 1.0 (such Macbook Pro with NVIDIA 9600M
     GT). Therefore, we flattened conditional statements. */
  bool isValid = true;
  if(gix < 0 || gix >= width) isValid = false;
  if(giy < 0 || giy >= height) isValid = false;
  if(giz < 0 || giz >= depth) isValid = false;

  if(isValid)
  {
  //smoothing along x direction
  dotx = 0;
  doty = 0;
  dotz = 0;
  for (i=0; i<ksize; i++)
    {
    ext = gix + i - radius;
    if (ext < 0 || ext >= width) continue;
    dotx += out[gidx+(i-radius)*DIM  ] * filter[i];
    doty += out[gidx+(i-radius)*DIM+1] * filter[i];
    dotz += out[gidx+(i-radius)*DIM+2] * filter[i];
    }
  swap[gidx]   = dotx;
  swap[gidx+1] = doty;
  swap[gidx+2] = dotz;

  //wait for x direction to be done
  barrier(CLK_GLOBAL_MEM_FENCE);

  //smoothing along y direction
  dotx = 0;
  doty = 0;
  dotz = 0;
  for (i=0; i<ksize; i++)
    {
    ext = giy + i - radius;
    if (ext < 0 || ext >= height) continue;
    dotx += swap[gidx+(i-radius)*width*DIM  ] * filter[i];
    doty += swap[gidx+(i-radius)*width*DIM+1] * filter[i];
    dotz += swap[gidx+(i-radius)*width*DIM+2] * filter[i];
    }
  out[gidx]   = dotx;
  out[gidx+1] = doty;
  out[gidx+2] = dotz;

  //wait for y direction to be done
  barrier(CLK_GLOBAL_MEM_FENCE);

  //smoothing along y direction
  dotx = 0;
  doty = 0;
  dotz = 0;
  for (i=0; i<ksize; i++)
    {
    ext = giz + i - radius;
    if (ext < 0 || ext >= depth) continue;
    dotx += out[gidx+(i-radius)*width*height*DIM  ] * filter[i];
    doty += out[gidx+(i-radius)*width*height*DIM+1] * filter[i];
    dotz += out[gidx+(i-radius)*width*height*DIM+2] * filter[i];
    }
  swap[gidx]   = dotx;
  swap[gidx+1] = doty;
  swap[gidx+2] = dotz;

  //wait for z direction to be done
  barrier(CLK_GLOBAL_MEM_FENCE);

  out[gidx]   = swap[gidx];
  out[gidx+1] = swap[gidx+1];
  out[gidx+2] = swap[gidx+2];
  }
}
#endif
