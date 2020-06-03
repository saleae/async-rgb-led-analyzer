#include "AsyncRgbLedAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "AsyncRgbLedAnalyzer.h"
#include "AsyncRgbLedAnalyzerSettings.h"
#include <iostream>
#include <fstream>

AsyncRgbLedAnalyzerResults::AsyncRgbLedAnalyzerResults( AsyncRgbLedAnalyzer* analyzer, AsyncRgbLedAnalyzerSettings* settings )
    : AnalyzerResults(), mSettings( settings ), mAnalyzer( analyzer )
{
}

AsyncRgbLedAnalyzerResults::~AsyncRgbLedAnalyzerResults()
{
}

void AsyncRgbLedAnalyzerResults::GenerateRGBStrings( const RGBValue& rgb, DisplayBase base, size_t bufSize, char* redBuf, char* greenBuff,
                                                     char* blueBuf )
{
    // generate a numerical representation of each color channel,
    // respecting the display-base setting
    AnalyzerHelpers::GetNumberString( rgb.red, base, mSettings->BitSize(), redBuf, bufSize );
    AnalyzerHelpers::GetNumberString( rgb.green, base, mSettings->BitSize(), greenBuff, bufSize );
    AnalyzerHelpers::GetNumberString( rgb.blue, base, mSettings->BitSize(), blueBuf, bufSize );
}

void AsyncRgbLedAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
    ClearResultStrings();
    Frame frame = GetFrame( frame_index );

    U32 ledIndex = frame.mData2;
    RGBValue rgb = RGBValue::CreateFromU64( frame.mData1 );

    // generate a Web/CSS representation of the color value
    U8 webColor[ 3 ];
    rgb.ConvertTo8Bit( mSettings->BitSize(), webColor );
    char webBuf[ 8 ];
    ::snprintf( webBuf, sizeof( webBuf ), "#%02x%02x%02x", webColor[ 0 ], webColor[ 1 ], webColor[ 2 ] );

    const int colorNumericBufferLength = 16;
    char redString[ colorNumericBufferLength ], greenString[ colorNumericBufferLength ], blueString[ colorNumericBufferLength ];

    GenerateRGBStrings( rgb, display_base, colorNumericBufferLength, redString, greenString, blueString );

    // generate four different string variants of varying length, starting with
    // the longest and decreasing in size
    char buf[ 256 ];

    // example: LED: 13 Red: 0x1A Green: 0x2B Blue: 0x3C #1A2B3C
    ::snprintf( buf, sizeof( buf ), "LED %d Red: %s Green: %s Blue: %s %s", ledIndex, redString, greenString, blueString, webBuf );
    AddResultString( buf );

    // example: 13 R:0x1A G:0x2B B:0x3C #1A2B3C
    ::snprintf( buf, sizeof( buf ), "%d R: %s G: %s B: %s %s", ledIndex, redString, greenString, blueString, webBuf );
    AddResultString( buf );

    // example: (13) #1A2B3C
    ::snprintf( buf, sizeof( buf ), "(%d) %s", ledIndex, webBuf );
    AddResultString( buf );

    // example: #1A2B3C
    AddResultString( webBuf );
}

void AsyncRgbLedAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
    std::ofstream file_stream( file, std::ios::out );

    U64 trigger_sample = mAnalyzer->GetTriggerSample();
    U32 sample_rate = mAnalyzer->GetSampleRate();

    file_stream << "Time [s], Packet ID, LED Index, Red, Green, Blue, Web-CSS" << std::endl;

    const U64 num_frames = GetNumFrames();

    for( U64 i = 0; i < num_frames; i++ )
    {
        const Frame frame = GetFrame( i );
        U64 packetId = GetPacketContainingFrameSequential( num_frames );

        if( packetId == INVALID_RESULT_INDEX )
        {
            packetId = -1;
        }

        char time_str[ 128 ];
        AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

        RGBValue rgb = RGBValue::CreateFromU64( frame.mData1 );

        // RGB numerical value representation
        const size_t bufSize = 16;
        char rs[ bufSize ], gs[ bufSize ], bs[ bufSize ];
        GenerateRGBStrings( rgb, display_base, bufSize, rs, gs, bs );

        // CSS representation
        U8 webColor[ 3 ];
        rgb.ConvertTo8Bit( mSettings->BitSize(), webColor );
        char webBuf[ 8 ];
        ::snprintf( webBuf, sizeof( webBuf ), "#%02x%02x%02x", webColor[ 0 ], webColor[ 1 ], webColor[ 2 ] );

        file_stream << time_str << "," << packetId << "," << frame.mData2 << "," << rs << "," << gs << "," << bs << "," << webBuf
                    << std::endl;

        if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
        {
            file_stream.close();
            return;
        }
    }

    file_stream.close();
}

void AsyncRgbLedAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
    Frame frame = GetFrame( frame_index );
    ClearTabularText();

    const U32 ledIndex = frame.mData2;
    const RGBValue rgb = RGBValue::CreateFromU64( frame.mData1 );

    const int colorNumericBufferLength = 8;
    char redString[ colorNumericBufferLength ], greenString[ colorNumericBufferLength ], blueString[ colorNumericBufferLength ];

    GenerateRGBStrings( rgb, display_base, colorNumericBufferLength, redString, greenString, blueString );

    // target content: [13] 0x1A, 0x2B, 0x3C
    char buf[ 64 ];
    ::snprintf( buf, 64, "[%d] %s, %s, %s", ledIndex, redString, greenString, blueString );
    AddTabularText( buf );
#endif
}

void AsyncRgbLedAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
    // not supported
}

void AsyncRgbLedAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
    // not supported
}
