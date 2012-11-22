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

#include "ESpeakApp.h"

#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/CardDoneMessage>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/DropDown>
#include <bb/cascades/Slider>
#include <bb/cascades/TextArea>
#include <bb/cascades/ToggleButton>
#include <QSettings>
#include <QDomDocument>
#include <bb/cascades/advertisement/Banner>

#include "speak_lib.h"

#include "ESpeak.h"

using namespace bb::cascades;
using namespace bb::system;
using namespace bb::cascades::advertisement;
using namespace ysal::eSpeakPlugin;

namespace ysal {
namespace eSpeakPlugin {

#define VOICE "voice"
#define SPEED "speed"
#define PITCH "pitch"
#define GENDER "gender"

ESpeakApp::ESpeakApp(bb::cascades::Application* app) :
		QObject(app) {

	_initDone = false;

	iManager = new InvokeManager(this);
	_speaker = new ESpeak(this);
	QmlDocument* qml;

	ApplicationStartupMode::Type startupMode = iManager->startupMode();

	switch (startupMode) {
	case ApplicationStartupMode::LaunchApplication:
		qml = QmlDocument::create("asset:///app.qml").parent(this);
		break;
	case ApplicationStartupMode::InvokeApplication:
	case ApplicationStartupMode::InvokeViewer:
	case ApplicationStartupMode::InvokeCard:
		qml = QmlDocument::create("asset:///card.qml").parent(this);
		QObject::connect(_speaker, SIGNAL(stopped()), this, SLOT(cardDone()));
		break;
	}
	qml->setContextProperty("speaker", _speaker);
	qml->setContextProperty("controller", this);

	// Listen to incoming invocation requests
	connect(iManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this,
			SLOT(handleInvoke(const bb::system::InvokeRequest&)));
	connect(iManager,
			SIGNAL(cardResizeRequested(const bb::system::CardResizeMessage&)),
			this, SLOT(resized(const bb::system::CardResizeMessage&)));
	connect(iManager, SIGNAL(cardPooled(const bb::system::CardDoneMessage&)),
			this, SLOT(pooled(const bb::system::CardDoneMessage&)));
	// create root object for the UI
	_root = qml->createRootObject<AbstractPane>();

	if (startupMode == ApplicationStartupMode::LaunchApplication) {
		this->loadSettings();
	}

	_initDone = true;

	app->setScene(_root);
}

ESpeakApp::~ESpeakApp() {
}

QString ESpeakApp::makeSSML(const QString &strData) {

	QSettings settings;
	QString strVoice = settings.value(VOICE, "english").toString();
	QString strSpeed = QString::number(settings.value(SPEED, 1.0).toFloat());
	QString strPitch = QString::number(settings.value(PITCH, 50).toInt());
	QString strGender = settings.value(GENDER, "female").toString();
	QByteArray output;
	QXmlStreamWriter stream(&output);
	stream.setAutoFormatting(false);
	stream.writeStartElement("speak");
	stream.writeStartElement("prosody");
	stream.writeAttribute("rate", strSpeed);
	stream.writeAttribute("pitch", strPitch);
	stream.writeStartElement("voice");
	stream.writeAttribute("name", strVoice);
	stream.writeAttribute("gender", strGender);
	stream.writeCharacters(strData);
	stream.writeEndElement(); // voice
	stream.writeEndElement(); // prosody
	stream.writeEndElement(); // speak

	return QString(output);
}

void ESpeakApp::handleInvoke(const bb::system::InvokeRequest& request) {

	QString data = QString::fromUtf8(request.data());

	QDomDocument xmlDoc;

	if (!xmlDoc.setContent(data)) {
		// Plain text?
		data = makeSSML(data);
	}

	_speaker->speak(data);
}

void ESpeakApp::resized(const bb::system::CardResizeMessage&) {
}

void ESpeakApp::pooled(const bb::system::CardDoneMessage&) {
}

void ESpeakApp::saveSettings() {

	if (_initDone) {
		QSettings settings;
		DropDown* voices = _root->findChild<DropDown*>("voice");
		QString strVoice = voices->selectedValue().toString();
		settings.setValue(VOICE, strVoice);
		Slider* speed = _root->findChild<Slider*>("speed");
		settings.setValue(SPEED, speed->value());
		Slider* pitch = _root->findChild<Slider*>("pitch");
		settings.setValue(PITCH, pitch->value());
		DropDown* gender = _root->findChild<DropDown*>("gender");
		QString strGender = gender->selectedValue().toString();
		settings.setValue(GENDER, strGender);
	}
}

void ESpeakApp::loadSettings() {

	QSettings settings;

	DropDown* voices = _root->findChild<DropDown*>("voice");
	QString strVoice = settings.value(VOICE, "english").toString();
	voices->setSelectedOption(voices->findChild<Option*>(strVoice));

	Slider *speed = _root->findChild<Slider*>("speed");
	speed->setValue(settings.value(SPEED, 1.0).toFloat());

	Slider *pitch = _root->findChild<Slider*>("pitch");
	pitch->setValue(settings.value(PITCH, 50).toInt());

	DropDown* gender = _root->findChild<DropDown*>("gender");
	QString strGender = settings.value(GENDER, "female").toString();
	gender->setSelectedOption(gender->findChild<Option*>(strGender));
}

void ESpeakApp::cardDone() {
	CardDoneMessage message;
	message.setData(tr("eSpeak finished"));
	message.setDataType("text/plain");
	message.setReason(tr(""));

	// Send message
	iManager->sendCardDone(message);
}

}
}
