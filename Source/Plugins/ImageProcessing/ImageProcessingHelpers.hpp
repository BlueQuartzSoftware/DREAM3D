#include <limits>
#include "itkImage.h"
#include "itkRegionalMaximaImageFilter.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkBinaryThresholdImageFunction.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"

namespace ImageProcessing
{
  namespace Functor
  {
    //gamma functor (doesn't seem to be implemented in itk)
    template< typename TPixel >
    class Gamma
    {
      public:
        Gamma() {}
        ~Gamma() {}
        bool operator!=( const Gamma& ) const
        {
          return false;
        }
        bool operator==( const Gamma& other ) const
        {
          return !(*this != other);
        }

        inline TPixel operator()(const TPixel& A, const TPixel& B) const
        {
          const double dA = static_cast< double >( A ) / std::numeric_limits<TPixel>::max();
          return static_cast< TPixel >( double(pow(dA, double(B))) * std::numeric_limits<TPixel>::max() );
        }
    };

    //custom functor to bring value within limits and round (without this functor itk add filter on an 8bit image 255+10->9)
    template< class TInput, class TOutput>
    class LimitsRound
    {
      public:
        LimitsRound() {}
        ~LimitsRound() {}
        bool operator!=( const LimitsRound& ) const
        {
          return false;
        }
        bool operator==( const LimitsRound& other ) const
        {
          return !(*this != other);
        }

        inline TOutput operator()(const TInput& A) const
        {
          const double dA = static_cast< double >( A );

          if(dA > std::numeric_limits<TOutput>::max())
          {
            return std::numeric_limits<TOutput>::max();
          }
          else if(dA < std::numeric_limits<TOutput>::min())
          {
            return std::numeric_limits<TOutput>::min();
          }

          //round if needed
          if(std::numeric_limits<TOutput>::is_integer && !std::numeric_limits<TInput>::is_integer)
          {
            if (dA >= floor(dA) + 0.5) { return static_cast< TOutput >(ceil(dA)); }
            else { return static_cast< TOutput >(floor(dA)); }
          }

          return static_cast< TOutput >( dA );
        }
    };


    //mean functor (doesn't seem to be currently implemented in itk)
    template< class TPixel>
    class Mean
    {
      public:
        Mean() {}
        ~Mean() {}
        bool operator!=(const Mean&) const
        {
          return false;
        }
        bool operator==(const Mean& other) const
        {
          return !( *this != other );
        }

        inline TPixel operator()(const TPixel& A, const TPixel& B) const
        {
          const double dA = static_cast< double >( A );
          const double dB = static_cast< double >( B );
          const double sum = dA + dB;

          return static_cast< TPixel >( sum / 2 );
        }
    };

    template< class TInput, class TOutput>
    class Luminance
    {
      public:
        Luminance() : weight_r(1.0), weight_g(1.0), weight_b(1.0) {}
        ~Luminance() {}

        bool operator!=( const Luminance & ) const
        {
          return false;
        }
        bool operator==( const Luminance & other ) const
        {
          return !(*this != other);
        }

        inline TOutput operator()(const TInput & A) const
        {
          return static_cast<TOutput>( A[0]*weight_r+A[1]*weight_g+A[2]*weight_b );
        }

        void SetRWeight(double r)
        {
          weight_r=r;
        }
        void SetGWeight(double g)
        {
          weight_g=g;
        }
        void SetBWeight(double b)
        {
          weight_b=b;
        }

      private:
        double weight_r;
        double weight_g;
        double weight_b;
    };

  }

  //this class emulates imagej's "find maxima" algorithm  
  template< class TInputImage >
  class LocalMaxima
  {
    public:
      typedef itk::Image<uint8_t, TInputImage::ImageDimension> BinaryImageType;
      typedef itk::RegionalMaximaImageFilter<TInputImage, BinaryImageType> MaximaType;
      typedef itk::BinaryImageToLabelMapFilter<BinaryImageType> BinaryToLabelType;
      typedef itk::BinaryThresholdImageFunction< TInputImage, double > ThresholdFunctionType;
      typedef itk::FloodFilledImageFunctionConditionalIterator< TInputImage, ThresholdFunctionType > FloodingIterator;

      typename std::vector<typename TInputImage::IndexType> static Find(typename TInputImage::Pointer inputImage, typename TInputImage::PixelType noiseTolerance, bool fullyConnected)
      {
        //find local maxaima (any region of constant value surrounded by pixels of lower value)
        typename MaximaType::Pointer maxima = MaximaType::New();
        maxima->SetInput(inputImage);
        maxima->SetBackgroundValue(0);
        maxima->SetForegroundValue(255);
        maxima->SetFullyConnected(fullyConnected);//4 vs 8 connected

        //segment local maxima flag image
        typename BinaryToLabelType::Pointer binaryLabel = BinaryToLabelType::New();
        binaryLabel->SetInput(maxima->GetOutput());
        binaryLabel->SetFullyConnected(fullyConnected);
        binaryLabel->Update();

        //loop over all local maxima eliminating bad peaks
        int numObjects = binaryLabel->GetOutput()->GetNumberOfLabelObjects();
        std::vector<bool> goodPeak (numObjects, true);
        for(int i=0; i<numObjects; i++)
        {
          //make sure we haven't already eliminated this peak
          if(goodPeak[i])
          {
            //get peak label object and height of peak
            typename BinaryToLabelType::OutputImageType::LabelObjectType* labelObject = binaryLabel->GetOutput()->GetNthLabelObject(i);
            
            //create list of seed points (label member pixels)
            typename std::vector<typename TInputImage::IndexType> seedList;
            for(int j=0; j<labelObject->Size(); j++)
            {
              seedList.push_back(labelObject->GetIndex(j));
            }

            //get peak value (all pixels in label have same value)
            typename TInputImage::PixelType peakValue = inputImage->GetPixel(seedList[0]);

            //create threshold function to flood fill
            typename ThresholdFunctionType::Pointer thresholdFunction = ThresholdFunctionType::New();
            thresholdFunction->SetInputImage(inputImage);
            thresholdFunction->ThresholdAbove(peakValue-noiseTolerance);//flood fill through anything within tolerance

            //iterate over image, flood filling (only changes pixels in iterator list, not image values)
            FloodingIterator it(inputImage, thresholdFunction, seedList);
            it.GoToBegin();
            while ( !it.IsAtEnd() )
            {
              //another peak of higher intensity is within the watershed tolerance, this peak is bad
              if(it.Get()>peakValue)
              {
                goodPeak[i]=false;
                break;
              }
              else if(it.Get()==peakValue)
              {
                //check if index belongs to another object (not in this peak)
                typename TInputImage::IndexType otherIndex = it.GetIndex();
                if(!labelObject->HasIndex(otherIndex))
                {
                  //there is another peak within tolerance that is the same intensity, find the peak it belongs to
                  //loop over other good objects to find peak id
                  for(int j = i+1; j < numObjects; j++)
                  {
                    if(goodPeak[j])
                    {
                      if(binaryLabel->GetOutput()->GetNthLabelObject(j)->HasIndex(otherIndex))
                      {
                        //label j is the peak with the same value as i, merge labels
                        goodPeak[j] = false;
                        typename BinaryToLabelType::OutputImageType::LabelObjectType* otherLabelObject = binaryLabel->GetOutput()->GetNthLabelObject(j);
                        for(int k=0; k<otherLabelObject->Size(); k++)
                        {
                          labelObject->AddIndex(otherLabelObject->GetIndex(k));
                        }
                        break;
                      }
                    }
                  }
                }
              }       
              //increment
              ++it;
            }
          }
        }

        //loop over all good peaks consolidating from a region->1 voxel
        std::vector<typename TInputImage::IndexType> peakLocations;
        for(int i=0; i<numObjects; i++)
        {
          if(goodPeak[i])
          {
            //get label object and find size
            typename BinaryToLabelType::OutputImageType::LabelObjectType* labelObject = binaryLabel->GetOutput()->GetNthLabelObject(i);
            int numVoxels = labelObject->Size();

            //find average location
            typename TInputImage::IndexType peakIndex;
            if(1==numVoxels)
            {
              for(int k=0; k<TInputImage::ImageDimension; k++)
              {
                peakIndex[k] = labelObject->GetIndex(0)[k];
              }
            }
            else
            {
              typename std::vector<float> avgIndex(TInputImage::ImageDimension, 0);
              for(int j=0; j<numVoxels; j++)
              {
                for(int k=0; k<TInputImage::ImageDimension; k++)
                {
                  avgIndex[k] = avgIndex[k] + labelObject->GetIndex(j)[k];
                }
              }
              for(int k=0; k<TInputImage::ImageDimension; k++)
              {
                avgIndex[k] = avgIndex[k] / numVoxels;
                peakIndex[k] = floor(avgIndex[k]);
                if(avgIndex[k]-peakIndex[k]>=0.5) peakIndex[k]++;
              }
            }
            peakLocations.push_back(peakIndex);
          }
        }

        return peakLocations;
      }
  };

}
