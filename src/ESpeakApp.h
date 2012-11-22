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

#ifndef ESpeakApp_H_
#define ESpeakApp_H_

#include <QObject>
#include "speak_lib.h"

namespace bb { namespace system { class InvokeManager; }}
namespace bb { namespace system { class InvokeRequest; }}
namespace bb { namespace system { class CardResizeMessage; }}
namespace bb { namespace system { class CardDoneMessage; }}
namespace bb { namespace cascades { class Application; }}
namespace bb { namespace cascades { class AbstractPane; }}

namespace ysal { namespace eSpeakPlugin { class ESpeak; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
namespace ysal {
namespace eSpeakPlugin {

class ESpeakApp : public QObject
{
    Q_OBJECT
public:
    ESpeakApp(bb::cascades::Application *app);
    virtual ~ESpeakApp();

    Q_INVOKABLE QString makeSSML(const QString &data);
    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void loadSettings();

private Q_SLOTS:
    // This slot is called whenever an invocation request is received
    void handleInvoke(const bb::system::InvokeRequest&);

    void resized(const bb::system::CardResizeMessage&);
    void pooled(const bb::system::CardDoneMessage&);
    void cardDone();

private:

    ESpeak *_speaker;

	bb::cascades::AbstractPane *_root;

    bb::system::InvokeManager *iManager;

    bool _initDone;
};

}
}

#endif /* ESpeakApp_H_ */
