#ifndef ASYNCRGBLED_ANALYZER_RESULTS
#define ASYNCRGBLED_ANALYZER_RESULTS

#include <AnalyzerResults.h>

#include "AsyncRgbLedHelpers.h" // for RGBValue

class AsyncRgbLedAnalyzer;
class AsyncRgbLedAnalyzerSettings;

class AsyncRgbLedAnalyzerResults : public AnalyzerResults
{
  public:
    AsyncRgbLedAnalyzerResults( AsyncRgbLedAnalyzer* analyzer, AsyncRgbLedAnalyzerSettings* settings );
    virtual ~AsyncRgbLedAnalyzerResults();

    void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base ) override;
    void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id ) override;

    void GenerateFrameTabularText( U64 frame_index, DisplayBase display_base ) override;
    void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base ) override;
    void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base ) override;

  protected: // functions
  protected: // vars
    AsyncRgbLedAnalyzerSettings* mSettings = nullptr;
    AsyncRgbLedAnalyzer* mAnalyzer = nullptr;

  private:
    void GenerateRGBStrings( const RGBValue& rgb, DisplayBase base, size_t bufSize, char* redBuf, char* greenBuff, char* blueBuf, char* whiteBuf );
};

#endif // ASYNCRGBLED_ANALYZER_RESULTS
