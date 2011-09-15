#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRigidVotingFilter.h"

int main(int argc, char *argv[])
{
  const unsigned int PairFeatureDimension=7;
  typedef itk::Vector<float,PairFeatureDimension> PairVectorType;
  typedef itk::Image<PairVectorType,1> PairVectorImageType;

}
