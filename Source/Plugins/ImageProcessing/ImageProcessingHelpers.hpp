


#include <limits>



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

}
