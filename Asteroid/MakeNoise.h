#ifndef MAKE_NOISE
#define MAKE_NOISE
// header contents
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <future>
#include <thread>
#include <iostream>
// required library
#pragma comment(lib, "winmm.lib")

class MakeNoise
{
    public:
        void playTrack(const int& time, const int& SamplesPerSec, int loop = 1);

	private:
		char* buffer;
		HWAVEOUT hWaveOut = NULL;
		WAVEFORMATEX wfx;
		// WORD  wFormatTag			Waveform-audio format type
		// WORD  nChannels			Number of channels in the waveform-audio data
		// DWORD nSamplesPerSec		Sample rate, in samples per second (hertz)
		// DWORD nAvgBytesPerSec	Average data-transfer rate - for WAVE_FORMAT_PCM, should be equal to nSamplesPerSec
		// WORD  nBlockAlign		Block alignment in bytes 
		// WORD  wBitsPerSample		Bits per sample - for WAVE_FORMAT_PCM, should be equal to 8 or 16
		// WORD  cbSize				Size, in bytes, of extra format information (ignored - 0)
}; 
// here we can write directly our algorithm so as to generate a track
inline char algorithm(int t) 
{ 
	return ((t & t + t / 256) - t * (t >> 15) & 64);
}

void MakeNoise::playTrack(const int& time, const int& samplesPerSec, int loop)
{	// allocate memory
	size_t size = static_cast<size_t>((time * samplesPerSec) / 1000);
	buffer = new char[size];
	// minimal instance
	if (loop < 1) loop = 1;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 0x01;
	wfx.nSamplesPerSec = samplesPerSec;
	wfx.nAvgBytesPerSec = samplesPerSec;
	wfx.nBlockAlign = 0x01;
	wfx.wBitsPerSample = 0x08;
	wfx.cbSize = NULL;
	// https://docs.microsoft.com/en-us/windows/win32/api/mmeapi/nf-mmeapi-waveoutopen
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, NULL, NULL, CALLBACK_NULL);
	// fills our buffer with your algorithm for track
	for (DWORD t = 0; t < size; t++)
		buffer[t] = static_cast<char>(algorithm(t));
	// header
	WAVEHDR header = { buffer, size, NULL, NULL, WHDR_BEGINLOOP | WHDR_ENDLOOP, loop, NULL, NULL };
	/*
	MMRESULT waveOutPrepareHeader/waveOutWrite/waveOutUnprepareHeader(
		HWAVEOUT  hwo,	// Handle to the waveform-audio output device
		LPWAVEHDR pwh,	// Pointer to a WAVEHDR structure that identifies the data block to be prepared
		UINT      cbwh	// Size, in bytes, of the WAVEHDR structure
	);
	*/
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR)); // clean up
	// The waveOutClose function closes the given waveform-audio output device
	waveOutClose(hWaveOut);
	/*
	MMRESULT waveOutSetVolume(
		HWAVEOUT hwo,		// Handle to the waveform-audio output device	
		DWORD    dwVolume	// set volume for speakers, 4 bits for each
	);
	*/
	waveOutSetVolume(hWaveOut, 0x0FFF0FFF); // set volume
	/*
	MMRESULT waveOutSetPlaybackRate(
		HWAVEOUT hwo,		// Handle to the waveform-audio output device	
		DWORD    dwRate		// New playback rate setting
	);
	*/
	waveOutSetPlaybackRate(hWaveOut, 0x0000FFFF); // pitch
	// sleep for a while - track is running
	Sleep((time * loop));
	// clean up
	delete[] buffer;
	std::cout << "The Thread Is Dead" << std::endl;
}

#endif