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

#ifndef __itkHDF5CommandRegionRecorder_h
#define __itkHDF5CommandRegionRecorder_h
#include "ITKIOHDF5Export.h"

#include <itkCommand.h>

namespace itk
{
/** \class HDF5CommandRegionRecorder
 *
 * \author David Froger
 *
 * \brief Records debug informations about HDF5 read/write operation.
 *
 * \ingroup ITKIOHDF5
 *
 * HDF5 supports reading and writing in a subregion of an multidimensional
 * array stored in a file via the use of the selectHyperslab method.
 * The HDF5 reader/writer module in ITK make use of this HDF5 feature to
 * implement ITK streaming/pasting.
 *
 * HDF5ImageIO invokes an IterationEvent when it modifies the m_H5RegionStart
 * and m_H5RegionSize arrays. These arrays, which are passed to the HDF5
 * functions, define the subregion to read/write. Adding this class as an
 * observer of the IterationEvent allows recording of the start of size of the
 * read/written regions, which provides useful informations to debug and
 * test the streaming/pasting capability of this module.
 */
class HDF5CommandRegionRecorder : public Command
{
public:
  /** Standard class typedefs. */
  typedef HDF5CommandRegionRecorder               Self;
  typedef Command                                 Superclass;
  typedef SmartPointer<HDF5CommandRegionRecorder> Pointer;

  /** HDF5 hsize_t */
  typedef HDF5ImageIO::hsize_t H5Size_t;

  /** Type of the start or size of a region*/
  typedef std::vector<H5Size_t> H5IndexType;

  /** Type of a list of H5IndexType*/
  typedef std::vector<H5IndexType> H5IndexHistoryType;

  /** Method for creation through the object factory. */
  itkNewMacro( HDF5CommandRegionRecorder );

protected:
  HDF5CommandRegionRecorder() {};

public:
  /** Record start and size of the current read/written region*/
  void Execute(Object *caller, const EventObject & event)
    {
    Execute( (const Object *)caller, event);
    }

  /** Record start and size of the current read/written region*/
  void Execute(const Object * object, const EventObject & event)
  {
    const HDF5ImageIO * filter = dynamic_cast< const HDF5ImageIO * >( object );
    if( !(IterationEvent().CheckEvent( &event )) )
      {
      return;
      }

      H5IndexType H5RegionStart;
      H5IndexType H5RegionSize;

      for (int i=0; i<filter->GetH5RegionDim(); i++)
        {
        H5RegionStart.push_back( filter->GetH5RegionStart()[i] );
        H5RegionSize.push_back( filter->GetH5RegionSize()[i] );
        }

      m_StartHistory.push_back( H5RegionStart );
      m_SizeHistory.push_back( H5RegionSize );
  }

  /** Print history of all the read/written regions */
  void PrintHistory(const H5IndexHistoryType &startHistory,
    const H5IndexHistoryType &sizeHistory,
    std::ostream &out)
  {
    for (unsigned it = 0; it < sizeHistory.size(); it++)
      {
      out << "Iteration: " << it << " ";
      H5IndexType H5RegionStart = startHistory[it];
      H5IndexType H5RegionSize = sizeHistory[it];
      out << "start: [";
      for (unsigned i = 0; i < H5RegionStart.size(); i++)
        {
          out << H5RegionStart[i] << " ";
        }
      out << "] Size: [";
      for (unsigned i = 0; i < H5RegionSize.size(); i++)
        {
          out << H5RegionSize[i] << " ";
        }
        out << "]" << std::endl;
      }
  }

  /** Print self history of all the read/written regions */
  void PrintSelfHistory(std::ostream &out)
  {
    this->PrintHistory(this->m_StartHistory, this->m_SizeHistory, out);
  }

  /** Compare history of all the read/written regions with expected one
   * \return Returns true if this history is as expected.
   */
  bool CheckHistory(const H5IndexHistoryType &expectedStartHistory,
    const H5IndexHistoryType &expectedSizeHistory,
    std::ostream &out)
  {
    if (m_StartHistory == expectedStartHistory and m_SizeHistory == expectedSizeHistory)
      {
      out << "As expected, history is: " << std::endl;
      this->PrintHistory(m_StartHistory, m_SizeHistory, out);
      return true;
      }
    else
      {
      out << "Expected history:" << std::endl;
      this->PrintHistory(expectedStartHistory, expectedSizeHistory, out);
      out << "But got:" << std::endl;
      this->PrintHistory(m_StartHistory, m_SizeHistory, out);
      return false;
      }
  }

private:
  /** Historic of the start of the read/write regions */
  H5IndexHistoryType m_StartHistory;

  /** Historic of the size of the read/write regions */
  H5IndexHistoryType m_SizeHistory;
};

} // end namespace itk

#endif // __itkHDF5CommandRegionRecorder_h
