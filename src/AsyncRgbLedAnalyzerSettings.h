#ifndef ASYNCRGBLED_ANALYZER_SETTINGS
#define ASYNCRGBLED_ANALYZER_SETTINGS

#include <vector>

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

#include "AsyncRgbLedHelpers.h"

class AsyncRgbLedAnalyzerSettings : public AnalyzerSettings
{
  public:
    AsyncRgbLedAnalyzerSettings();
    virtual ~AsyncRgbLedAnalyzerSettings();

    // delete all the copy/move operators for rule-of-5 compliance
    AsyncRgbLedAnalyzerSettings( const AsyncRgbLedAnalyzerSettings& ) = delete;
    AsyncRgbLedAnalyzerSettings( AsyncRgbLedAnalyzerSettings&& ) = delete;
    const AsyncRgbLedAnalyzerSettings& operator=( const AsyncRgbLedAnalyzerSettings& ) = delete;
    AsyncRgbLedAnalyzerSettings& operator=( AsyncRgbLedAnalyzerSettings&& ) = delete;


    bool SetSettingsFromInterfaces() override;
    void UpdateInterfacesFromSettings();
    void LoadSettings( const char* settings ) override;
    const char* SaveSettings() override;

    enum Controller
    {
        LED_WS2811 = 0,
        LED_WS2812B,
        LED_WS2813,
        LED_TM1809,
        LED_TM1804,
        LED_UCS1903,
        LED_LPD1886_8bit,
        LED_LPD1886_12bit
    };

    Controller mLEDController = LED_WS2811;
    Channel mInputChannel = UNDEFINED_CHANNEL;

    /// bits ber LED channel, either 8 or 12 at present
    U8 BitSize() const;

    /// LED channel count, either 3 (RGB) or 9 (three RGB outputs) at present
    U8 LEDChannelCount() const;

    bool IsHighSpeedSupported() const;

    BitTiming DataTiming( BitState value, bool isHighSpeed = false ) const;

    TimingTolerance ResetTiming() const;

    ColorLayout GetColorLayout() const;

  protected:
    void InitControllerData();

    std::unique_ptr<AnalyzerSettingInterfaceChannel> mInputChannelInterface;
    std::unique_ptr<AnalyzerSettingInterfaceNumberList> mControllerInterface;

    // we can't do direct defualt initialisation here, since according to C++11
    // that makes this type non-POD and hence unsuitable for direct initialisation.
    // C++14 fixes this.
    struct LedControllerData
    {
        std::string mName;
        std::string mDescription;
        U8 mBitsPerChannel; // = 8;
        U8 mChannelCount;   // = 3;
        TimingTolerance mResetTiming;
        BitTiming mDataTiming[ 2 ]; // BIT_HIGH and BIT_LOW

        bool mHasHighSpeed;                  // = true
        BitTiming mDataTimingHighSpeed[ 2 ]; // BIT_HIGH and BIT_LOW

        ColorLayout mLayout; // = LAYOUT_RGB
    };

    std::vector<LedControllerData> mControllers;
};

#endif // ASYNCRGBLED_ANALYZER_SETTINGS
