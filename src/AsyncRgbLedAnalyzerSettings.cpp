#include "AsyncRgbLedAnalyzerSettings.h"

#include <cassert>

#include <AnalyzerHelpers.h>

const char* DEFAULT_CHANNEL_NAME = "Addressable LEDs (Async)";

double operator"" _ns( unsigned long long x )
{
    return x * 1e-9;
}

double operator"" _us( unsigned long long x )
{
    return x * 1e-6;
}

AsyncRgbLedAnalyzerSettings::AsyncRgbLedAnalyzerSettings()
{
    InitControllerData();

    mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mInputChannelInterface->SetTitleAndTooltip( "LED Channel", "Standard Addressable LEDs (Async)" );
    mInputChannelInterface->SetChannel( mInputChannel );

    mControllerInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mControllerInterface->SetTitleAndTooltip( "LED Controller", "Specify the LED controller in use." );

    int index = 0;

    for( const auto& controllerData : mControllers )
    {
        mControllerInterface->AddNumber( index++, controllerData.mName.c_str(), controllerData.mDescription.c_str() );
    }

    mControllerInterface->SetNumber( mLEDController );

    AddInterface( mInputChannelInterface.get() );
    AddInterface( mControllerInterface.get() );

    AddExportOption( 0, "Export as text/csv file" );
    AddExportExtension( 0, "text", "txt" );
    AddExportExtension( 0, "csv", "csv" );

    ClearChannels();
    AddChannel( mInputChannel, DEFAULT_CHANNEL_NAME, false );
}

AsyncRgbLedAnalyzerSettings::~AsyncRgbLedAnalyzerSettings()
{
}

void AsyncRgbLedAnalyzerSettings::InitControllerData()
{
    // order of values here must correspond to the Controller enum
    mControllers = {
        // name, description, bits per channel, channels per frame, reset time nsec, low-speed data nsec, has high speed, high speed data
        // nsec, color layout

        // https://cdn-shop.adafruit.com/datasheets/WS2811.pdf
        { "WS2811",
          "Worldsemi 24-bit RGB controller",
          8,
          3,
          { 50_us, 50_us, 50_us },
          {
              // low-speed times
              { { 350_ns, 500_ns, 650_ns }, { 1850_ns, 2000_ns, 2150_ns } },    // 0-bit times
              { { 1050_ns, 1200_ns, 1350_ns }, { 1150_ns, 1300_ns, 1450_ns } }, // 1-bit times
          },
          true,
          {
              // high-speed times
              { { 175_ns, 250_ns, 325_ns }, { 925_ns, 1000_ns, 1075_ns } }, // 0-bit times
              { { 525_ns, 600_ns, 675_ns }, { 1225_ns, 1300_ns, 1375_ns } } // 1-bit times
          },
          LAYOUT_RGB },
        // https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
        // http://www.seeedstudio.com/document/pdf/WS2812B%20Datasheet.pdf
        { "WS2812B",
          "Worldsemi 24-bit RGB integrated light-source",
          8,
          3,
          { 50_us, 50_us, 50_us },
          {
              // low-speed times
              { { 200_ns, 400_ns, 550_ns }, { 700_ns, 850_ns, 1050_ns } }, // 0-bit times
              { { 650_ns, 800_ns, 1050_ns }, { 200_ns, 450_ns, 600_ns } }, // 1-bit times
          },
          false,
          { {}, {} },
          LAYOUT_GRB },

        // http://www.led-color.com/upload/201609/WS2813%20LED.pdf
        { "WS2813",
          "Worldsemi 24-bit RGB integrated light-source",
          8,
          3,
          { 50_us, 50_us, 50_us },
          {
              // low-speed times
              { { 300_ns, 375_ns, 450_ns }, { 300_ns, 875_ns, 100_us } },  // 0-bit times
              { { 750_ns, 875_ns, 1000_ns }, { 300_ns, 375_ns, 100_us } }, // 1-bit times
          },
          false,
          { {}, {} },
          LAYOUT_GRB },

        // https://www.deskontrol.net/descargas/datasheets/TM1809.pdf
        { "TM1809",
          "Titan Micro 9-chanel 24-bit RGB controller",
          8,
          9,
          { 24_us, 24_us, 1.0 },
          {
              // low-speed times
              { { 450_ns, 600_ns, 750_ns }, { 1050_ns, 1200_ns, 1350_ns } }, // 0-bit times
              { { 1050_ns, 1200_ns, 1350_ns }, { 450_ns, 600_ns, 750_ns } }, // 1-bit times
          },
          true,
          {
              // high-speed times
              { { 250_ns, 320_ns, 390_ns }, { 530_ns, 600_ns, 670_ns } }, // 0-bit times
              { { 530_ns, 600_ns, 670_ns }, { 250_ns, 320_ns, 390_ns } }  // 1-bit times
          },
          LAYOUT_RGB },

        // https://www.deskontrol.net/descargas/datasheets/TM1804.pdf
        { "TM1804",
          "Titan Micro 24-bit RGB controller",
          8,
          3,
          { 10_us, 10_us, 1.0 },
          {
              // low-speed times
              { { 850_ns, 1_us, 1150_ns }, { 1850_ns, 2_us, 2150_ns } }, // 0-bit times
              { { 1850_ns, 2_us, 2150_ns }, { 850_ns, 1_us, 1150_ns } }, // 1-bit times
          },
          false,
          { {}, {} },
          LAYOUT_RGB },

        // http://www.bestlightingbuy.com/pdf/UCS1903%20datasheet.pdf
        { "UCS1903",
          "UCS1903 24-bit RGB controller",
          8,
          3,
          { 24_us, 24_us, 1.0 },
          {
              // low-speed times
              { { 350_ns, 500_ns, 650_ns }, { 1850_ns, 2000_ns, 2150_ns } }, // 0-bit times
              { { 1850_ns, 2000_ns, 2150_ns }, { 350_ns, 500_ns, 650_ns } }, // 1-bit times
          },
          true,
          {
              // high-speed times
              { { 175_ns, 250_ns, 325_ns }, { 925_ns, 1000_ns, 1075_ns } }, // 0-bit times
              { { 925_ns, 1000_ns, 1075_ns }, { 175_ns, 250_ns, 325_ns } }  // 1-bit times
          },
          LAYOUT_RGB },

        // https://www.syncrolight.co.uk/datasheets/LPD1886%20datasheet.pdf
        { "LPD1886 - 24 bit",
          "LPD1886 RGB controller in 24-bit mode",
          8,
          3,
          { 24_us, 30_us, 1.0 },
          {
              // low-speed times
              { { 150_ns, 200_ns, 280_ns }, { 500_ns, 600_ns, 10_us } }, // 0-bit times
              { { 450_ns, 600_ns, 9_us }, { 150_ns, 200_ns, 10_us } },   // 1-bit times
          },
          false,
          { {}, {} },
          LAYOUT_RGB },

        { "LPD1886 - 36 bit",
          "LPD1886 RGB controller in 36-bit mode",
          12,
          3,
          { 24_us, 30_us, 1.0 },
          {
              // low-speed times
              { { 150_ns, 200_ns, 280_ns }, { 500_ns, 600_ns, 10_us } }, // 0-bit times
              { { 450_ns, 600_ns, 9_us }, { 150_ns, 200_ns, 10_us } },   // 1-bit times
          },
          false,
          { {}, {} },
          LAYOUT_RGB },

        // https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf
        { "SK6812 RGBW",
          "24-bit RGBW integrated light-source",
          8,
          4,
          { 50_us, 50_us, 50_us },
          {
              // low-speed times
              { { 200_ns, 400_ns, 550_ns }, { 700_ns, 850_ns, 1050_ns } }, // 0-bit times
              { { 650_ns, 800_ns, 1050_ns }, { 200_ns, 450_ns, 600_ns } }, // 1-bit times
          },
          false,
          { {}, {} },
          LAYOUT_GRB },

    };
}

bool AsyncRgbLedAnalyzerSettings::SetSettingsFromInterfaces()
{
    mInputChannel = mInputChannelInterface->GetChannel();
    // explicit cast to keep MSVC happy
    const int index = static_cast<int>( mControllerInterface->GetNumber() );
    mLEDController = static_cast<Controller>( index );

    ClearChannels();
    AddChannel( mInputChannel, DEFAULT_CHANNEL_NAME, true );

    return true;
}

void AsyncRgbLedAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mInputChannelInterface->SetChannel( mInputChannel );
    mControllerInterface->SetNumber( mLEDController );
}

void AsyncRgbLedAnalyzerSettings::LoadSettings( const char* settings )
{
    SimpleArchive text_archive;
    text_archive.SetString( settings );

    U32 controllerInt;
    text_archive >> mInputChannel;
    text_archive >> controllerInt;
    mLEDController = static_cast<Controller>( controllerInt );

    ClearChannels();
    AddChannel( mInputChannel, DEFAULT_CHANNEL_NAME, true );

    UpdateInterfacesFromSettings();
}

const char* AsyncRgbLedAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << mInputChannel;
    text_archive << mLEDController;

    return SetReturnString( text_archive.GetString() );
}

U8 AsyncRgbLedAnalyzerSettings::BitSize() const
{
    return mControllers.at( mLEDController ).mBitsPerChannel;
}

U8 AsyncRgbLedAnalyzerSettings::LEDChannelCount() const
{
    return mControllers.at( mLEDController ).mChannelCount;
}

bool AsyncRgbLedAnalyzerSettings::IsHighSpeedSupported() const
{
    return mControllers.at( mLEDController ).mHasHighSpeed;
}

BitTiming AsyncRgbLedAnalyzerSettings::DataTiming( BitState value, bool isHighSpeed ) const
{
    const auto& c = mControllers.at( mLEDController );
    assert( !isHighSpeed || c.mHasHighSpeed );

    return isHighSpeed ? c.mDataTimingHighSpeed[ value ] : c.mDataTiming[ value ];
}

TimingTolerance AsyncRgbLedAnalyzerSettings::ResetTiming() const
{
    return mControllers.at( mLEDController ).mResetTiming;
}

ColorLayout AsyncRgbLedAnalyzerSettings::GetColorLayout() const
{
    return mControllers.at( mLEDController ).mLayout;
}
