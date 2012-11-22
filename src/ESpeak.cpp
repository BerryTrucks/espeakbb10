/*****************************************************************************
*	Copyright (C) 2012 by Yuri Salnikov                                      *
*	                                                                         *
*	This file is part of Aloud - text-to-speech program for BlackBerry 10    *
*	platform based on eSpeak open source project                             *
*	(http://espeak.sourceforge.net/).                                        *
*                                                                            *
*   Aloud is free software: you can redistribute it and/or modify            *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation, either version 3 of the License, or        *
*   (at your option) any later version.                                      *
*                                                                            *
*   Aloud is distributed in the hope that it will be useful,                 *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
*   GNU General Public License for more details -                            *
*	<http://www.gnu.org/licenses/>                                           *
/*****************************************************************************/

#include <stdlib.h>
#include <QDataStream>
#include "ESpeak.h"
#include <bb/multimedia/MediaPlayer.hpp>
#include <pthread.h>

using namespace bb::multimedia;

namespace ysal {
namespace eSpeakPlugin {

int ESpeak::_samplerate;
unsigned int ESpeak::_samples_total = 0;
unsigned int ESpeak::_samples_split = 0;
unsigned int ESpeak::_samples_split_seconds = 0;
unsigned int ESpeak::_wavefile_count = 0;

QByteArray ESpeak::_wavData;

// Error message function for ALUT.
static void reportALUTError() {

	fprintf(stdout, "ALUT reported the following error: %s\n", alutGetErrorString(alutGetError()));
	fflush(stdout);
}

// Error message function for OpenAL.
static void reportOpenALError() {
	fprintf(stdout, "OpenAL reported the following error: %s\n", alutGetErrorString(alutGetError()));
	fflush(stdout);
}

ESpeak::ESpeak(QObject *parent) :
		QObject(parent) {

	init();
}

ESpeak::~ESpeak() {

	espeak_Terminate();

	// Clear all the sources.
	for (int sourceIndex = 0; sourceIndex < MAX_NBR_OF_SOUND_SOURCES;
			sourceIndex++) {
		ALuint source = _soundSources[sourceIndex];
		alDeleteSources(1, &source);

		if (alGetError() != AL_NO_ERROR) {
			reportOpenALError();
		}
	}

	// Exit the ALUT.
	if (alutExit() == false) {
		reportALUTError();
	}
}

void ESpeak::init() {

	_samples_total = 0;
	_samples_split = 0;
	_samples_split_seconds = 0;
	_wavefile_count = 0;

	_source = 0;

	QString path = getenv("HOME");
	path = path.left(path.lastIndexOf('/'));
	path += QString("/app/native/assets");

	const char *data_path = path.toStdString().c_str();

	const char* szVersionInfo = espeak_Info(NULL);

	printf("Espeak version: %s\n", szVersionInfo);
	_samplerate = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 600, data_path,
			0);

	// Initialize the ALUT.
	if (alutInit(NULL, NULL) == false) {
		reportALUTError();
	}

	// Generate a number of sources used to attach buffers and play the sounds.
	alGenSources(MAX_NBR_OF_SOUND_SOURCES, _soundSources);

	if (alGetError() != AL_NO_ERROR) {
		reportOpenALError();
	}

}

void ESpeak::Write4Bytes(QByteArray &data, int value, int pos) {

	int ix;

	for (ix = 0; ix < 4; ix++) {
		if (pos < data.size()) {
			data[pos] = (value & 0xff);
		} else {
			data.append(value & 0xff);
		}
		pos++;
		value = value >> 8;
	}
}

int ESpeak::OpenWavFile(int rate) {

	static unsigned char wave_hdr[44] = { 'R', 'I', 'F', 'F', 0x24, 0xf0, 0xff,
			0x7f, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ', 0x10, 0, 0, 0, 1, 0,
			1, 0, 9, 0x3d, 0, 0, 0x12, 0x7a, 0, 0, 2, 0, 0x10, 0, 'd', 'a', 't',
			'a', 0x00, 0xf0, 0xff, 0x7f };

	_wavData.clear();

	_wavData.append((const char*) wave_hdr, 24);
	Write4Bytes(_wavData, rate, _wavData.size());
	Write4Bytes(_wavData, rate * 2, _wavData.size());
	_wavData.append((const char*) &wave_hdr[32], 12);

	return (0);
}

void ESpeak::CloseWavFile() {
	unsigned int pos;

	pos = _wavData.size();

	Write4Bytes(_wavData, pos - 8, 4);

	Write4Bytes(_wavData, pos - 44, 40);

}

int ESpeak::eSpeakCallback(short *wav, int numsamples, espeak_EVENT *events) {

	if (wav == NULL) {

		CloseWavFile();
		return (0);
	}

	while (events->type != 0) {
		if (events->type == espeakEVENT_SAMPLERATE) {
			_samplerate = events->id.number;
			_samples_split = _samples_split_seconds * _samplerate;
		} else if (events->type == espeakEVENT_SENTENCE) {
			// start a new WAV file when the limit is reached, at this sentence boundary
			if ((_samples_split > 0) && (_samples_total > _samples_split)) {
				CloseWavFile();
				_samples_total = 0;
				_wavefile_count++;
			}
		}
		events++;
	}

	if (_wavData.size() == 0) {
		if (OpenWavFile(_samplerate) != 0)
			return (1);
	}

	if (numsamples > 0) {

		_samples_total += numsamples;

		_wavData.append((const char*) wav, numsamples * 2);
	}
	return (0);
}

static void* listener_start(void* arg) {

	ESpeak *speaker = (ESpeak*)arg;
	while (speaker->playing());
	speaker->emitStopped();
	return NULL;
}

bool ESpeak::speak(QString ssml) {

	_wavData.clear();

	float pitch = 1.0f;
	float gain = 1.0f;

	espeak_SetSynthCallback(eSpeakCallback);

	if (EE_OK
			== espeak_Synth(ssml.toStdString().c_str(), ssml.length(), 0,
					POS_CHARACTER, 0, espeakSSML | espeakCHARS_UTF8, NULL,
					NULL)) {

		ALuint bufferID = alutCreateBufferFromFileImage(_wavData.constData(),
				_wavData.size());

		if (alutGetError() != ALUT_ERROR_NO_ERROR) {

			reportALUTError();
		}

		int sourceIndex = 0;

		if (bufferID != 0) {
			// Increment which source we are using, so that we play in a "free" source.
			sourceIndex = (sourceIndex + 1) % MAX_NBR_OF_SOUND_SOURCES;

			// Get the source in which the sound will be played.
			_source = _soundSources[sourceIndex];

			if (alIsSource(_source) == AL_TRUE) {

				// Attach the buffer to an available source.
				alSourcei(_source, AL_BUFFER, bufferID);

				if (alGetError() != AL_NO_ERROR) {
					reportOpenALError();
					return false;
				}

				// Set the source pitch value.
				alSourcef(_source, AL_PITCH, pitch);

				if (alGetError() != AL_NO_ERROR) {
					reportOpenALError();
					return false;
				}

				// Set the source gain value.
				alSourcef(_source, AL_GAIN, gain);

				if (alGetError() != AL_NO_ERROR) {
					reportOpenALError();
					return false;
				}

				emit started();

				// Play the source.
				alSourcePlay(_source);

				pthread_t thread = 0;
				pthread_create(&thread, NULL, listener_start, (void*)this);

				if (alGetError() != AL_NO_ERROR) {
					reportOpenALError();
					return false;
				}
			}

			if (bufferID != 0) {
				alDeleteBuffers(1, &bufferID);

				if (alGetError() != AL_NO_ERROR) {
					reportOpenALError();
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

void ESpeak::stop() {
	alSourceStop(_source);
}

bool ESpeak::playing() {
	if (_source == 0) {
		return false;
	}
	else {
		ALint val;
		alGetSourcei(_source, AL_SOURCE_STATE, &val);
		return  (val == AL_PLAYING);
	}
}

}
}
