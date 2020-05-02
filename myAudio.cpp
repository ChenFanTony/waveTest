#include "myAudio.h"

// buffer size for 10s
DWORD bufSize = 44100 * 10L;
myAudio *myAudio::ptrthis = NULL;
FILE* myAudio::fp = NULL;
const char *filename = "myvioce.pcm";

baseMyAudio::baseMyAudio(int _buffNum)
	:buffNum(_buffNum)
{

}

baseMyAudio::~baseMyAudio()
{

}

myAudio::~myAudio()
{

}
void CALLBACK myAudio::waveInBack(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	LPWAVEHDR pwhd = (PWAVEHDR)dwParam1;
	switch (uMsg)
	{
	case WIM_OPEN:
		//cout << "Begin recording..." << endl;
		break;
	case WIM_DATA:
		//cout << "recive..." << endl;
		//TB: we can non-block write the data
		fwrite(pwhd->lpData,1, pwhd->dwBufferLength,  fp);
		waveInUnprepareHeader(hwi, pwhd, sizeof(WAVEHDR));
		waveInPrepareHeader(hwi, pwhd, sizeof(WAVEHDR));
		waveInAddBuffer(hwi, pwhd, sizeof(WAVEHDR));

		break;
	case WIM_CLOSE:
		cout << "close wim" << endl;
		waveInReset(hwi);
		waveInUnprepareHeader(hwi, pwhd, sizeof(WAVEHDR));
		cout << " End Recording" << endl;
		//waveInClose(hwi);
		break;
	default:
		cout << " wavein back function error" << endl;
		break;
	}
}

void CALLBACK myAudio::waveOutBack(HWAVEOUT hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	LPWAVEHDR pwhr = (PWAVEHDR)dwParam1;
	switch (uMsg)
	{
	case WOM_CLOSE:
		cout << "waveout close" << endl;
		waveOutReset(hwi);
		waveOutUnprepareHeader(hwi, pwhr, sizeof(WAVEHDR));
		waveOutClose(hwi);

		fclose(fp);
		break;
	case WOM_DONE:
		//cout << "play " << endl;
		//TB: we can read the data once into the memory
		fread(pwhr->lpData, 1,pwhr->dwBufferLength,fp);
		waveOutPrepareHeader(hwi, pwhr, sizeof(WAVEHDR));
		waveOutWrite(hwi, pwhr, sizeof(WAVEHDR));
		break;
	case WOM_OPEN:
		//cout << "BEGIN PLAYING" << endl;
		break;
	default:
		cout << " waveout back function error" << endl;
		break;
	}
}
bool myAudio::recordAudio()
{
	makeSettings();

	wavehdr = new WAVEHDR*[buffNum];
	for (int i = 0;i < buffNum;++i)
	{
		wavehdr[i] = new WAVEHDR;
	}
	for (int i = 0;i < buffNum;++i)
	{
		wavehdr[i]->lpData = (LPSTR)(pBuff[i]);
		wavehdr[i]->dwBufferLength = bufSize;
		wavehdr[i]->dwBytesRecorded = 0;
		wavehdr[i]->dwUser = 0;
		wavehdr[i]->dwFlags = 0;
		wavehdr[i]->dwLoops = 1;
		wavehdr[i]->lpNext = NULL;
		wavehdr[i]->reserved = 0;
	}

	fp = fopen(filename, "wb+");

	MMRESULT opnRet = waveInOpen(&waveInputDev, WAVE_MAPPER, &mywaveform, (DWORD_PTR)waveInBack, 0, CALLBACK_FUNCTION);
	if (opnRet != MMSYSERR_NOERROR){
		cout << "waveInOpen error" << endl;
		return false;
	}
	for (int i=0;i<buffNum;++i){
		MMRESULT prepareRet1 = waveInPrepareHeader(waveInputDev, wavehdr[i], sizeof(WAVEHDR));
		if (prepareRet1 != MMSYSERR_NOERROR) {
			cout << "waveInPrepareHeader error" << endl;
			return false;
		}
	}

	for (int i = 0;i < buffNum;++i) {
		MMRESULT addBufRet1 = waveInAddBuffer(waveInputDev, wavehdr[i], sizeof(WAVEHDR));
		if (addBufRet1 != MMSYSERR_NOERROR) {
			cout << "waveInAddBuffer error" << endl;
			return false;
		}
	}

	MMRESULT startResult = waveInStart(waveInputDev);
	if (startResult != MMSYSERR_NOERROR) {
		cout << "waveInStart error" << endl;
		return false;
	}
	return true;
}

bool myAudio::stopRecord()
{
	waveInClose(waveInputDev);
	waveInStop(waveInputDev);
	if (fp != NULL) {
		fclose(fp);
	}

	return true;
}

bool myAudio::stopPlayer()
{
	waveOutPause(waveOutDev);
	if (fp != NULL){
		fclose(fp);
	}
	return true;
}

bool myAudio::playerAudio()
{
	makeSettings();
	fp = fopen(filename, "rb");

	wavehdrout = new WAVEHDR*[buffNum];
	for (int i = 0;i < buffNum;++i)
	{
		wavehdrout[i] = new WAVEHDR;
	}
	for (int i = 0;i < buffNum;++i) {
		wavehdrout[i]->lpData = (LPSTR)pBuff[i];
		wavehdrout[i]->dwBufferLength = bufSize;
		wavehdrout[i]->dwBytesRecorded = 0;
		wavehdrout[i]->dwUser = 0;
		wavehdrout[i]->dwFlags = 0;
		wavehdrout[i]->dwLoops = 1;
		wavehdrout[i]->lpNext = NULL;
		wavehdrout[i]->reserved = 0;
	}

	if (fp == NULL)
	{
		cout << "player open file error" << endl;
		return false;
	}

	waveOutOpen(&waveOutDev,WAVE_MAPPER, &mywaveform, (DWORD_PTR)waveOutBack, 0, CALLBACK_FUNCTION);

	for (int i=0;i<buffNum;++i){
		if (0 == (fread(wavehdrout[i]->lpData, 1, wavehdrout[i]->dwBufferLength, fp))) {
			cout << "fread error" << endl;
			return false;
		}
		waveOutPrepareHeader(waveOutDev, wavehdrout[i], sizeof(WAVEHDR));
	}

	for (int i = 0;i < buffNum;++i) {
		waveOutWrite(waveOutDev, wavehdrout[i], sizeof(WAVEHDR));
	}

	cout << "wave out success" << endl;
	return true;
}

bool myAudio::releaseSource()
{
	for (int i = 0;i < buffNum;++i) {
		if (wavehdr[i])
		{
			delete[]wavehdr[i];
			wavehdr[i] = NULL;
		}
	}
	delete[]wavehdr;
	wavehdr = NULL;

	for (int i = 0;i < buffNum;++i) {
		if (wavehdrout[i])
		{
			delete[]wavehdrout[i];
			wavehdrout[i] = NULL;
		}
	}
	delete[]wavehdrout;
	wavehdrout = NULL;

	for (int i = 0;i < buffNum;++i) {
		if (pBuff[i])
		{
			delete[]pBuff[i];
			pBuff[i] = NULL;
		}
	}
	delete[]pBuff;
	pBuff = NULL;
	cout << "release success" << endl;
	return true;
}

bool  myAudio::makeSettings()
{
	mywaveform.wFormatTag = WAVE_FORMAT_PCM;
	mywaveform.nChannels = 2;
	mywaveform.nSamplesPerSec = 44100;
	mywaveform.wBitsPerSample = 16;
	mywaveform.cbSize = 0;
	mywaveform.nBlockAlign = (mywaveform.wBitsPerSample * mywaveform.nChannels) >> 3;
	mywaveform.nAvgBytesPerSec = mywaveform.nBlockAlign * mywaveform.nSamplesPerSec;

	pBuff = new BYTE*[buffNum];
	for (int i = 0;i < buffNum;++i)
	{
		pBuff[i] = new BYTE[bufSize];
	}
	return true;
}
