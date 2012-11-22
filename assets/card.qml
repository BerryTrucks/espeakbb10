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

import bb.cascades 1.0

Page {
    Container {
        layout: DockLayout {
        }
        Container {
            layout: StackLayout {
            }
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            ActivityIndicator {
                running: true
                horizontalAlignment: HorizontalAlignment.Center
                minWidth: 300.0
                minHeight: 300.0
            }
            Button {
                text: qsTr("Stop")
                onClicked: {
                    speaker.stop();
                }
                horizontalAlignment: HorizontalAlignment.Center
            }
        }
    }
}
