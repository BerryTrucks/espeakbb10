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

#ifndef ESPEAK_H_
#define ESPEAK_H_

#include <qobject.h>
#include "speak_lib.h"
#include <QByteArray>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

// The number of max number of sound sources
#define MAX_NBR_OF_SOUND_SOURCES 32

namespace ysal {
namespace eSpeakPlugin {

class ESpeak: public QObject {
	Q_OBJECT
public:
	ESpeak(QObject *parent);
	virtual ~ESpeak();

	void init();

	Q_INVOKABLE bool speak(QString ssml);
	Q_INVOKABLE void stop();

	Q_PROPERTY(bool playing READ playing);

	static int eSpeakCallback(short *buf, int size, espeak_EVENT *event);

	bool playing();
	void emitStopped() {
		emit stopped();
	}

signals:

	void started();
	void stopped();

private:

	static void Write4Bytes(QByteArray &data, int value, int pos);
	static int OpenWavFile(int rate);
	static void CloseWavFile();

	static int _samplerate;

	static unsigned int _samples_total;
	static unsigned int _samples_split;
	static unsigned int _samples_split_seconds;
	static unsigned int _wavefile_count;

	static QByteArray _wavData;

    // Sound sources
    ALuint _soundSources[MAX_NBR_OF_SOUND_SOURCES];

    ALuint _source;
};

}
}

#endif /* ESPEAK_H_ */
