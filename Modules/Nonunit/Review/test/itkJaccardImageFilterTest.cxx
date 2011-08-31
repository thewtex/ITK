// Jaccard filter example
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkJaccardImageFilter.h"
#include "stdio.h"

int itkJaccardImageFilterTest(int argc, char *argv[])
{
    typedef itk::Image<unsigned char, 2>   ImageType;
    typedef itk::JaccardImageFilter<ImageType>  FilterType;

    typedef itk::ImageFileReader<ImageType> ReaderType;


    if (argc !=3)
    {
        //correct usage
        cerr << "Error in usage."<<endl;
        cerr << " ImageFilter [inputA] [inputB]"<< endl;
        cerr << " The input files must be binary."<< endl;
    }
    else
    {
        //read Images
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(argv[1]);
        reader->Update();


        ReaderType::Pointer reader2 = ReaderType::New();
        reader2->SetFileName(argv[2]);
        reader2->Update();

        FilterType::Pointer filter = FilterType::New();
        filter->SetInputImage1( reader->GetOutput() );
        filter->SetInputImage2( reader2->GetOutput() );
        filter->Update();
        cerr <<" The value output by the  Jaccard filter is : "<<
            filter->getJaccardDistance()<<endl;

        typedef  itk::ImageFileWriter< ImageType  > WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName("TestOutput.jpg");
        writer->SetInput(filter->GetOutput());
        writer->Update();

        return EXIT_SUCCESS;
    }
}
