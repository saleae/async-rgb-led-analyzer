#ifndef ASYNCRGBLED_ANALYZER_H
#define ASYNCRGBLED_ANALYZER_H

#include <Analyzer.h>

#include "AsyncRgbLedSimulationDataGenerator.h"
#include "AsyncRgbLedHelpers.h"

// forward decls
class AsyncRgbLedAnalyzerSettings;
class AsyncRgbLedAnalyzerResults;

class AsyncRgbLedAnalyzer : public Analyzer2
{
  public:
    AsyncRgbLedAnalyzer();
    virtual ~AsyncRgbLedAnalyzer();

    void SetupResults() override;
    void WorkerThread() override;

    U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels ) override;
    U32 GetMinimumSampleRateHz() override;

    const char* GetAnalyzerName() const override;
    bool NeedsRerun() override;

  protected: // vars
    std::unique_ptr<AsyncRgbLedAnalyzerSettings> mSettings;
    std::unique_ptr<AsyncRgbLedAnalyzerResults> mResults;
    AnalyzerChannelData* mChannelData = nullptr;

    AsyncRgbLedSimulationDataGenerator mSimulationDataGenerator;
    bool mSimulationInitialized = false;

    // analysis vars:
    double mSampleRateHz = 0;

    // minimum valid low time for a data bit, in either speed mode supported
    // by the controller.
    double mMinimumLowDurationSec = 0.0;

    bool mFirstBitAfterReset = false;
    bool mDidDetectHighSpeed = false;

  private:
    struct RGBResult
    {
        bool mValid = false;
        bool mIsReset = false;
        RGBValue mRGB;
        U64 mValueBeginSample = 0;
        U64 mValueEndSample = 0;
    };

    RGBResult ReadRGBTriple();

    struct ReadResult
    {
        bool mValid = false;
        bool mIsReset = false;
        BitState mBitValue = BIT_LOW;
        U64 mBeginSample = 0;
        U64 mEndSample = 0;
    };

    ReadResult ReadBit();
    void SynchronizeToReset();

    bool DetectSpeedMode( double positiveTimeSec, double negativeTimeSec, BitState& value );
};

extern "C"
{
    ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
    ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer();
    ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );
}

#endif // ASYNCRGBLED_ANALYZER_H
