#ifndef ASYNCRGBLED_ANALYZER_HELPERS
#define ASYNCRGBLED_ANALYZER_HELPERS

#include <AnalyzerTypes.h>

enum ColorLayout
{
    LAYOUT_RGB = 0,
    LAYOUT_GRB
};

struct RGBValue
{
    RGBValue() = default;
    ~RGBValue() = default;

    RGBValue( U16 r, U16 g, U16 b ) : red( r ), green( g ), blue( b )
    {
        ;
    }

    U16 red = 0;
    U16 green = 0;
    U16 blue = 0;
    U16 padding = 0; // no alpha in LED colors

    void ConvertToControllerOrder( ColorLayout layout, U16* values ) const;

    static RGBValue CreateFromControllerOrder( ColorLayout layout, U16* values );

    static RGBValue CreateFromU64( U64 raw );

    U64 ConvertToU64() const;

    /**
     * @brief ConvertTo8Bit - adjust precision to three 8-bit values compatible
     * witha  web / CSS color specification.
     * @param bitSize - bits used in this RGB value, eg 8, 10, 12 or 16
     * @param values - array of three U8s to store output web color value
     */
    void ConvertTo8Bit( U8 bitSize, U8* values ) const;
};

struct TimingTolerance
{
    TimingTolerance() = default;

    TimingTolerance( double minS, double nomS, double maxS ) : mMinimumSec( minS ), mNominalSec( nomS ), mMaximumSec( maxS )
    {
        ;
    }

    double mMinimumSec = 0.0;
    double mNominalSec = 0.0;
    double mMaximumSec = 0.0;

    bool WithinTolerance( const double t ) const;
};

struct BitTiming
{
    BitTiming()
    {
        ;
    }

    BitTiming( const TimingTolerance& pt, const TimingTolerance& nt ) : mPositiveTiming( pt ), mNegativeTiming( nt )
    {
        ;
    }

    TimingTolerance mPositiveTiming;
    TimingTolerance mNegativeTiming;

    bool WithinTolerance( const double positiveTime, const double negativeTime ) const;
};

#endif // of #define ASYNCRGBLED_ANALYZER_SETTINGS
