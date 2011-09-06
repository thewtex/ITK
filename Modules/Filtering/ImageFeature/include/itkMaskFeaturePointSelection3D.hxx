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
#ifndef __itkMaskFeaturePointSelection3D_hxx
#define __itkMaskFeaturePointSelection3D_hxx

#include "itkMaskFeaturePointSelection3D.h"
#include "itkImageRegionIterator.h"
#include <vector>
#include <algorithm>

namespace itk
{
  /**
   *
   */
  template<class TRealCoor, class TImagePixel, class TMaskPixel>
    MaskFeaturePointSelection3D<TRealCoor, TImagePixel, TMaskPixel>::MaskFeaturePointSelection3D()
    {
      // defaults
      m_BlockNonConnexityType = 26;
      m_BlockHalfSize.Fill(2);
      m_BlockRejectedFraction = 0.9;

      // Modify superclass default values, can be overridden by subclasses
      this->SetNumberOfRequiredInputs(1);

      PointDataContainerPointer pointData = PointDataContainer::New();
      OutputPointSetPointer pointset = this->GetOutput();
      pointset->SetPointData(pointData.GetPointer());
    }

  /**
   *
   */
  template<class TRealCoor, class TImagePixel, class TMaskPixel>
    MaskFeaturePointSelection3D<TRealCoor, TImagePixel, TMaskPixel>::~MaskFeaturePointSelection3D()
    {
    }

  /**
   *
   */
  template<class TRealCoor, class TImagePixel, class TMaskPixel>
    void
    MaskFeaturePointSelection3D<TRealCoor, TImagePixel, TMaskPixel>::PrintSelf(
        std::ostream & os, Indent indent) const
    {
      Superclass::PrintSelf(os, indent);
      os << indent << "m_BlockNonConnexityType: " << m_BlockNonConnexityType
          << std::endl << indent << "m_BlockHalfSize: " << m_BlockHalfSize[0]
          << "x" << m_BlockHalfSize[1] << "x" << m_BlockHalfSize[1] << std::endl
          << indent << "m_BlockRejectedFraction: " << m_BlockRejectedFraction
          << std::endl;
    }

  /**
   *
   */
  template<class TRealCoor, class TImagePixel, class TMaskPixel>
    void
    MaskFeaturePointSelection3D<TRealCoor, TImagePixel, TMaskPixel>::GenerateOutputInformation()
    {
    }

  /**
   *
   */
  template<class TRealCoor, class TImagePixel, class TMaskPixel>
    void
    MaskFeaturePointSelection3D<TRealCoor, TImagePixel, TMaskPixel>::SetInputImage(
        const InputImageType *inputImage)
    {
      // This const_cast is needed due to the lack of
      // const-correctness in the ProcessObject.
      this->SetNthInput(0, const_cast<InputImageType *>(inputImage));
    }

  /**
   *
   */
  template<class TRealCoor, class TImagePixel, class TMaskPixel>
    void
    MaskFeaturePointSelection3D<TRealCoor, TImagePixel, TMaskPixel>::SetMaskImage(
        const MaskImageType *maskImage)
    {
      // This const_cast is needed due to the lack of
      // const-correctness in the ProcessObject.
      this->SetNthInput(1, const_cast<MaskImageType *>(maskImage));
    }

  /**
   *
   */
  template<class TRealCoor, class TImagePixel, class TMaskPixel>
    void
    MaskFeaturePointSelection3D<TRealCoor, TImagePixel, TMaskPixel>::SetBlockNonConnexityType(
        int connex)
    {
      if (connex != 6 && connex != 18 && connex != 26)
        itkExceptionMacro(
            "Cannot set connexity to " << connex << ", allowed 6, 18 and 26.");

      m_BlockNonConnexityType = connex;
    }

  /**
   *
   */
  template<class TRealCoor, class TImagePixel, class TMaskPixel>
    void
    MaskFeaturePointSelection3D<TRealCoor, TImagePixel, TMaskPixel>::SetBlockRejectedFraction(
        double fraction)
    {
      m_BlockRejectedFraction = fraction;
    }

  /**
   *
   */
  template<class TRealCoor, class TImagePixel, class TMaskPixel>
    void
    MaskFeaturePointSelection3D<TRealCoor, TImagePixel, TMaskPixel>::GenerateData(
        void)
    {
      std::vector < FeaturePointType > pointList;

      OutputPointSetPointer pointSet = this->GetOutput();
      InputImageConstPointer image = this->GetInput(0);
      MaskImageConstPointer mask = this->GetInput(1);

      PointsContainerPointer points = PointsContainer::New();
      PointDataContainerPointer pointData = PointDataContainer::New();

      FeaturePointType feature;

      // Neighborhood iterator with windows size m_BlockHalfSize + 1 + m_BlockHalfSize
      InputImageIterator itImage(m_BlockHalfSize, image,
          image->GetRequestedRegion());

      typedef typename InputImageIterator::NeighborIndexType NeighborIndexType;
      NeighborIndexType numPixelsInNeighborhood = itImage.Size();

      MaskImageIterator itMask(mask, mask->GetRequestedRegion());

      // initialize selectionBlockMap
      typedef Image<unsigned char, 3> MapImageType;
      typedef typename MapImageType::PixelType MapImagePixelType;
      typedef typename MapImageType::IndexType MapImageIndexType;
      typedef ImageRegionIterator<MaskImageType> MapImageIterator;

      MapImageType::Pointer selectionBlockMap = MapImageType::New();
      selectionBlockMap->SetRegions(image->GetRequestedRegion());
      selectionBlockMap->Allocate();

      MapImageIterator itMap(selectionBlockMap,
          selectionBlockMap->GetRequestedRegion());

      // compute mean and variance for eligible points
      for (itImage.GoToBegin(), itMask.GoToBegin(), itMap.GoToBegin();
          !itImage.IsAtEnd(); ++itImage, ++itMask, ++itMap)
            {
        if (itMask.Get())
          {
            feature.idx = itImage.GetIndex();
            image->TransformIndexToPhysicalPoint(feature.idx, feature.coor);

            double m = 0.0, v = 0.0;
            for (NeighborIndexType i = 0; i < numPixelsInNeighborhood; i++)
              {
              InputImagePixelType pixel = itImage.GetPixel(i);
              m += pixel;
              v += pixel * pixel;
            }
          feature.mean = m / numPixelsInNeighborhood;
          feature.variance = v / ((double) numPixelsInNeighborhood)
              - feature.mean * feature.mean;
          feature.selected = false;

          pointList.push_back(feature);
          itMap.Set(1);
        }
      else
        itMap.Set(0);
    }

// sort wrt variance
  std::sort(pointList.begin(), pointList.end());

  unsigned pointsLeft = round(
      pointList.size() * (1.0 - m_BlockRejectedFraction));

// iterate backwards: from higher to lower variance
  for (typename std::vector<FeaturePointType>::reverse_iterator rit =
      pointList.rbegin(); rit < pointList.rend(); ++rit)
    {
    (*rit).selected = selectionBlockMap->GetPixel((*rit).idx) && pointsLeft;
    if ((*rit).selected)
      {
        // add point to pointset
        points->push_back((*rit).coor);
        pointData->push_back((*rit).idx);

        pointsLeft--;

        if (m_BlockNonConnexityType == 6)
          {
            int connexOff[6][3] =
              {
                { 1, 0, 0 },
                { 0, 1, 0 },
                { 0, 0, 1 },
                { -1, 0, 0 },
                { 0, -1, 0 },
                { 0, 0, -1 } };
            for (int j = 0; j < m_BlockNonConnexityType; j++)
              {
              MapImageIndexType idx = (*rit).idx;
              idx[0] += connexOff[j][0];
              idx[1] += connexOff[j][1];
              idx[2] += connexOff[j][2];
              selectionBlockMap->SetPixel(idx, 0);
            }
        }

      else if (m_BlockNonConnexityType == 18)
        {
          int connexOff[18][3] =
            {
              { 1, 0, 0 },
              { 1, 1, 0 },
              { 1, 0, 1 },
              { 1, -1, 0 },
              { 1, 0, -1 },
              { 0, 1, 0 },
              { 0, 1, 1 },
              { -1, 1, 0 },
              { 0, 1, -1 },
              { 0, 0, 1 },
              { -1, 0, 1 },
              { 0, -1, 1 },
              { -1, 0, 0 },
              { -1, -1, 0 },
              { -1, 0, -1 },
              { 0, -1, 0 },
              { 0, -1, -1 },
              { 0, 0, -1 } };
          for (int j = 0; j < m_BlockNonConnexityType; j++)
            {
            MapImageIndexType idx = (*rit).idx;
            idx[0] += connexOff[j][0];
            idx[1] += connexOff[j][1];
            idx[2] += connexOff[j][2];
            selectionBlockMap->SetPixel(idx, 0);
          }
      }
    else
      {
        for (int j = -1; j < 2; j++)
          for (int k = -1; k < 2; k++)
            for (int l = -1; l < 2; l++)
              {
              MapImageIndexType idx = (*rit).idx;
              idx[0] += j;
              idx[1] += k;
              idx[2] += l;
              selectionBlockMap->SetPixel(idx, 0);
            }
    }
}
}

 // set points
pointSet->SetPoints(points);
pointSet->SetPointData(pointData);

 // This indicates that the current BufferedRegion is equal to the
 // requested region. This action prevents useless rexecutions of
 // the pipeline.
pointSet->SetBufferedRegion(pointSet->GetRequestedRegion());
}
} // end namespace itk

#endif
