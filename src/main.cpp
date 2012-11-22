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

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/advertisement/Banner>

#include <QLocale>
#include <QTranslator>
#include "ESpeakApp.h"

using namespace bb::cascades;


int main(int argc, char **argv)
{
	qmlRegisterType<bb::cascades::advertisement::Banner>("bb.cascades.advertisement", 1, 0, "Banner");
    // this is where the server is started etc
    Application app(argc, argv);

    // localization support
    QTranslator translator;
    QString locale_string = QLocale().name();
    QString filename = QString( "eSpeakPlugin_%1" ).arg( locale_string );
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }

    new ysal::eSpeakPlugin::ESpeakApp(&app);

    // we complete the transaction started in the app constructor and start the client event loop here
    return Application::exec();
    // when loop is exited the Application deletes the scene which deletes all its children (per qt rules for children)
}
