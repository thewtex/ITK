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

OutPixelType Functor(float lowerThreshold, float upperThreshold,
                     OutPixelType insideValue, OutPixelType outsideValue,
                     float currentValue)
{
  if ( lowerThreshold <= currentValue && currentValue <= upperThreshold )
  {
    return insideValue;
  }
  return outsideValue;
}

#ifdef DIM_1
__kernel void BinaryThresholdFilter(InPixelType lowerThreshold, InPixelType upperThreshold,
                                    OutPixelType insideValue, OutPixelType outsideValue,
                                    __global const InPixelType* in, __global OutPixelType* out,
                                    int width)
{
  int gix = get_global_id(0);
  if(gix < width)
  {
    out[gix] = Functor(lowerThreshold, upperThreshold, insideValue, outsideValue, in[gix]);
  }
}
#endif


#ifdef DIM_2
__kernel void BinaryThresholdFilter(InPixelType lowerThreshold, InPixelType upperThreshold,
                                    OutPixelType insideValue, OutPixelType outsideValue,
                                    __global const InPixelType* in, __global OutPixelType* out,
                                    int width, int height)
{
  int gix = get_global_id(0);
  int giy = get_global_id(1);
  if(gix < width && giy < height)
  {
    unsigned int gidx = width*giy + gix;
    out[gidx] = Functor(lowerThreshold, upperThreshold, insideValue, outsideValue, in[gidx]);
  }
}
#endif

#ifdef DIM_3
__kernel void BinaryThresholdFilter(InPixelType lowerThreshold, InPixelType upperThreshold,
                                    OutPixelType insideValue, OutPixelType outsideValue,
                                    __global const InPixelType* in, __global OutPixelType* out,
                                    int width, int height, int depth)
{
  int gix = get_global_id(0);
  int giy = get_global_id(1);
  int giz = get_global_id(2);

  bool isValid;
  if(gix < width) isValid = true;
  else if(giy < height) isValid = true;
  else if(giz < depth) isValid = true;
  else isValid = false;

  if( isValid )
  {
    unsigned int gidx = width*(giz*height + giy) + gix;
    out[gidx] = Functor(lowerThreshold, upperThreshold, insideValue, outsideValue, in[gidx]);
  }
}
#endif
