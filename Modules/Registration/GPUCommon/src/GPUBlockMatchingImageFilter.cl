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

__kernel void BlockMatchingFilter(
  const __global FIXEDTYPE *fixedImage,    // in: fixed image
  const __global MOVINGTYPE *movingImage,  // in: moving image
  const __global uint *features,           // in: coordinates of feature points
  __global uint *displacements,            // out: new coordinates
  __global SIMTYPE *similarities,          // out: similarities of new points
  const uint4 imageSize,                   // in: dimensions of images
  const uint4 blockRadius,                 // in: block radius
  const uint4 searchRadius,                // in: search window radius
  __local FIXEDTYPE *windowBuffer,         // tmp: window buffer in fixed image
  __local MOVINGTYPE *blockBuffer,         // tmp: block buffer in moving image
  __local uint *localDisplacements,        // tmp: new points with max sim
  __local SIMTYPE *localSimilarities       // tmp: max sim for each workitem
)
{
  size_t groupId = get_group_id( 0 ); // feature
  size_t localSize = get_local_size( 0 );
  size_t localId  = get_local_id( 0 );

  // coordinates of feature processed by this workgroup
  uint4 feature = ( uint4 )
    (
    features[ groupId * 3 ],
    features[ groupId * 3 + 1 ],
    features[ groupId * 3 + 2 ],
    0
    );

  // size of local buffer for fixed image search window with border
  uint4 windowBufferSize = ( searchRadius + blockRadius ) * 2 + 1;
  uint windowBufferVoxelCount = windowBufferSize.x
                              * windowBufferSize.y
                              * windowBufferSize.z;

  // load window buffer
  for ( uint i = localId; i < windowBufferVoxelCount; i += localSize )
    {
    // compute buffer XYZ coordinates from offset ( localId )
    uint4 coor = ( uint4 )
      (
      i % ( windowBufferSize.y * windowBufferSize.x ) % windowBufferSize.x,
      i % ( windowBufferSize.y * windowBufferSize.x ) / windowBufferSize.x,
      i / ( windowBufferSize.y * windowBufferSize.x ),
      0
      );

    // convert buffer coordinates into image coordinates
    coor += feature - searchRadius - blockRadius;

    // copy image voxel to buffer voxel
      windowBuffer[ i ] = fixedImage[ (coor.z * imageSize.y + coor.y ) * imageSize.x + coor.x ];
    }

  // size of local buffer for moving image block
  uint4 blockBufferSize = blockRadius * 2 + 1;
  uint blockBufferVoxelCount = blockBufferSize.x
                             * blockBufferSize.y
                             * blockBufferSize.z;

  // load block buffer
  for ( uint i = localId; i < blockBufferVoxelCount; i += localSize )
  {
    // compute buffer XYZ coordinates from offset ( localId )
    uint4 coor = ( uint4 )
      (
      i % ( blockBufferSize.y * blockBufferSize.x ) % blockBufferSize.x,
      i % ( blockBufferSize.y * blockBufferSize.x ) / blockBufferSize.x,
      i / ( blockBufferSize.y * blockBufferSize.x ),
      0
      );

    // convert buffer coordinates into image coordinates
    coor += feature - blockRadius;

    // copy image voxel to buffer voxel
    blockBuffer[ i ] = movingImage[ (coor.z * imageSize.y + coor.y ) * imageSize.x + coor.x ];
  }

  // finished preloading into local memory
  barrier( CLK_LOCAL_MEM_FENCE );


  // size of local search window ( no border )
  uint4 windowSize = searchRadius * 2 + 1;
  uint windowVoxelCount = windowSize.x
                        * windowSize.y
                        * windowSize.z;

  // max similarity for this workitem
  SIMTYPE similarity = 0.0;
  uint4 newLocation = ( uint4 )( 0, 0, 0, 0 );

  // loop over fixed image window in local buffer
  for ( uint i = localId; i < windowVoxelCount; i += localSize )
    {
    // compute window XYZ coordinates from offset ( localId )
    uint4 coor = ( uint4 )
      (
      i % ( windowSize.y * windowSize.x ) % windowSize.x,
      i % ( windowSize.y * windowSize.x ) / windowSize.x,
      i / ( windowSize.y * windowSize.x ),
      0
      );

    // center of block in fixed image
    coor += blockRadius;

    // new location block in window buffer ( fixed image )
    uint4 fixedBlockFrom = coor - blockRadius;
    uint4 fixedBlockTo = coor + blockRadius;

    SIMTYPE fixedSum = 0.0;
    SIMTYPE fixedSumOfSquares = 0.0;
    SIMTYPE movingSum = 0.0;
    SIMTYPE movingSumOfSquares = 0.0;
    SIMTYPE covariance = 0.0;

    // loop over blocks
    for ( uint fixedBlockZ = fixedBlockFrom.z, movingIdx = 0; fixedBlockZ <= fixedBlockTo.z; fixedBlockZ++ )
      {
      for ( uint fixedBlockY = fixedBlockFrom.y; fixedBlockY <= fixedBlockTo.y; fixedBlockY++ )
        {
        for ( uint fixedBlockX = fixedBlockFrom.x; fixedBlockX <= fixedBlockTo.x; fixedBlockX++, movingIdx++ )
          {
          // inside block loop
          SIMTYPE fixedValue = windowBuffer[ ( fixedBlockZ * windowBufferSize.y + fixedBlockY ) * windowBufferSize.x + fixedBlockX ];
          SIMTYPE movingValue = blockBuffer[ movingIdx ]; // 0..1..blockBufferVoxelCount-1

          fixedSum += fixedValue;
          fixedSumOfSquares += fixedValue * fixedValue;

          movingSum += movingValue;
          movingSumOfSquares += movingValue * movingValue;

          covariance += fixedValue * movingValue;
          }
        }
      }

    SIMTYPE fixedMean = fixedSum / blockBufferVoxelCount;
    SIMTYPE movingMean = movingSum / blockBufferVoxelCount;
    SIMTYPE fixedVariance = fixedSumOfSquares - blockBufferVoxelCount * fixedMean * fixedMean;
    SIMTYPE movingVariance = movingSumOfSquares - blockBufferVoxelCount * movingMean * movingMean;
    covariance -= fixedMean * movingMean * blockBufferVoxelCount;

    SIMTYPE sim = 0.0;
    if ( fixedVariance * movingVariance )
      {
      sim = ( covariance * covariance ) / ( fixedVariance * movingVariance );
      }

    if ( sim > similarity )
      {
      similarity = sim;
      newLocation = coor;
      }
    }

  newLocation += feature - searchRadius - blockRadius;
  localSimilarities[ localId ] = similarity;
  localDisplacements[ localId * 3 ] = newLocation.x;
  localDisplacements[ localId * 3 + 1 ] = newLocation.y;
  localDisplacements[ localId * 3 + 2 ] = newLocation.z;

  // finished computing local similarities / displacements
  barrier( CLK_LOCAL_MEM_FENCE );


  // reduce to one max value per workgroup
  for ( uint offset = localSize / 2; offset > 0; offset >>= 1 )
    {
    if ( localId < offset )
      {
      if ( localSimilarities[ localId + offset ] > localSimilarities[ localId ] )
        {
        localSimilarities[ localId ] = localSimilarities[ localId + offset ];;
        localDisplacements[ localId * 3 ] = localDisplacements[ ( localId + offset ) * 3 ];
        localDisplacements[ localId * 3 + 1 ] = localDisplacements[ ( localId + offset ) * 3 + 1 ];
        localDisplacements[ localId * 3 + 2 ] = localDisplacements[ ( localId + offset ) * 3 + 2 ];
        }
      }

      barrier( CLK_LOCAL_MEM_FENCE );
    }


  // write final values to global memory
  if ( !localId )
    {
    similarities[ groupId ] = localSimilarities[ 0 ];
    displacements[ groupId * 3 ] = localDisplacements[ 0 ];
    displacements[ groupId * 3 + 1 ] = localDisplacements[ 1 ];
    displacements[ groupId * 3 + 2 ] = localDisplacements[ 2 ];
    }

}
