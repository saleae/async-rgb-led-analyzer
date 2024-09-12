#include "AsyncRgbLedSimulationDataGenerator.h"

#include <cmath> // for M_PI, cos
#include <iostream>
#include <cassert>

#include "AsyncRgbLedAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

AsyncRgbLedSimulationDataGenerator::AsyncRgbLedSimulationDataGenerator()
{
}

AsyncRgbLedSimulationDataGenerator::~AsyncRgbLedSimulationDataGenerator()
{
}

void AsyncRgbLedSimulationDataGenerator::Initialize( U32 simulation_sample_rate, AsyncRgbLedAnalyzerSettings* settings )
{
    // Initialize the random number generator with a literal seed to obtain repeatability
    // Change this for srand(time(NULL)) for "truly" random sequences
    // NOTICE rand() an srand() are *not* thread safe
    srand( 42 );

    mSimulationSampleRateHz = simulation_sample_rate;
    mSettings = settings;

    mMaximumChannelValue = ( 1 << mSettings->BitSize() ) - 1;

    // TODO pass in the analyzer and call GetMinimumSampleRate?
    const double clockFrequencyUnused = 1.0;
    mClockGenerator.Init( clockFrequencyUnused, mSimulationSampleRateHz );

    mLEDSimulationData.SetChannel( mSettings->mInputChannel );
    mLEDSimulationData.SetSampleRate( simulation_sample_rate );
    mLEDSimulationData.SetInitialBitState( BIT_LOW );

    if( mSettings->IsHighSpeedSupported() )
    {
        // check if the requested sample rate is high enough
        if( mSimulationSampleRateHz > 18000000 )
        {
            mDoGenerateHighSpeedMode = true;
        }
        else
        {
            std::cerr << "Disabling high-speed data generation due to simulated sample rate" << std::endl;
        }
    }
}

U32 AsyncRgbLedSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate,
                                                                SimulationChannelDescriptor** simulation_channel )
{
    U64 adjusted_largest_sample_requested =
        AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

    while( mLEDSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
    {
        WriteReset();

        // six RGB-triple cascade between resets, i.e six discrete LEDs
        // or two of the 3-LED combined drivers. We could perhaps make
        // this adjustable
        for( int t = 0; t < 6; ++t )
        {
            CreateRGBWord();
        }

        ++mFrameCount;

        // toggle high-speed mode every seven frames if it's supported
        if( ( ( mFrameCount % 7 ) == 0 ) && mDoGenerateHighSpeedMode )
        {
            mHighSpeedMode = !mHighSpeedMode;
        }
    }

    *simulation_channel = &mLEDSimulationData;
    return 1;
}

void AsyncRgbLedSimulationDataGenerator::CreateRGBWord()
{
    const RGBValue rgb = RandomRGBValue();
    WriteRGBTriple( rgb );
}

void AsyncRgbLedSimulationDataGenerator::WriteRGBTriple( const RGBValue& rgb )
{
    U16 values[ 3 ];
    rgb.ConvertToControllerOrder( mSettings->GetColorLayout(), values );

    for( int i = 0; i < 3; ++i )
    {
        WriteUIntData( values[ i ], mSettings->BitSize() );
    }
}

void AsyncRgbLedSimulationDataGenerator::WriteReset()
{
    assert( mLEDSimulationData.GetCurrentBitState() == BIT_LOW );
    const double resetSec = mClockGenerator.AdvanceByTimeS( mSettings->ResetTiming().mNominalSec );
    mLEDSimulationData.Advance( resetSec );
    // and stay low
}

void AsyncRgbLedSimulationDataGenerator::WriteUIntData( U16 data, U8 bit_count )
{
    BitExtractor extractor( data, AnalyzerEnums::MsbFirst, bit_count );

    for( U32 bit = 0; bit < bit_count; ++bit )
    {
        WriteBit( extractor.GetNextBit() );
    }
}

void AsyncRgbLedSimulationDataGenerator::WriteBit( bool b )
{
    assert( mLEDSimulationData.GetCurrentBitState() == BIT_LOW );

    const BitState bs = b ? BIT_HIGH : BIT_LOW;
    const BitTiming& timing = mSettings->DataTiming( bs, mHighSpeedMode );

    const double highSamples = mClockGenerator.AdvanceByTimeS( timing.mPositiveTiming.mNominalSec );
    const double lowSamples = mClockGenerator.AdvanceByTimeS( timing.mNegativeTiming.mNominalSec );

    mLEDSimulationData.Transition(); // go high
    mLEDSimulationData.Advance( highSamples );
    mLEDSimulationData.Transition(); // go low
    mLEDSimulationData.Advance( lowSamples );
}

RGBValue AsyncRgbLedSimulationDataGenerator::RandomRGBValue() const
{
    const U16 red = rand() % mMaximumChannelValue;
    const U16 green = rand() % mMaximumChannelValue;
    const U16 blue = rand() % mMaximumChannelValue;
    return RGBValue{ red, green, blue, 0 };
}
