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
import bb.cascades.advertisement 1.0

// creates one page with a label

NavigationPane {
    Page {
        resizeBehavior: PageResizeBehavior.None
        Container {
            layout: StackLayout {
            }
            Container {
                verticalAlignment: VerticalAlignment.Top
                horizontalAlignment: HorizontalAlignment.Center
                layout: StackLayout {
                }
                leftPadding: 20.0
                rightPadding: 20.0
                topPadding: 20.0
                
                Banner {
                    zoneId: 117145
                    refreshRate: 60
                    preferredWidth: 320
                    preferredHeight: 50
                    transitionsEnabled: true
                    backgroundColor: Color.Gray
                    horizontalAlignment: HorizontalAlignment.Center
                }
                
                TextArea {
                    id: data
                    objectName: "data"
                    hintText: qsTr("Enter text to say")
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Button {
                    id: speakButton
                    property string textInactive: qsTr("Say it")
                    property string textActive: qsTr("Stop")
                    function onSpeakerStarted() {
                        speakButton.text = textActive;
                    }
                    function onSpeakerStopped() {
                        speakButton.text = textInactive;
                    }
                    text: textInactive
                    onClicked: {
                        speaker.started.connect(speakButton.onSpeakerStarted);
                        speaker.stopped.connect(speakButton.onSpeakerStopped);
                        if (speaker.playing) {
                            speaker.stop();
                        } else {
                            speaker.speak(controller.makeSSML(data.text));
                        }
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                }
                DropDown {
                    id: voice
                    objectName: "voice"
                    title: qsTr("Language")
                    enabled: true
                    onSelectedOptionChanged: {
                        controller.saveSettings();
                    }
                    Option {
                        objectName: "english"
                        imageSource: "asset:///images/uk.png"
                        text: qsTr("English UK")
                        value: "english"
                        selected: true
                    }
                    Option {
                        objectName: "english-us"
                        imageSource: "asset:///images/us.png"
                        text: qsTr("English US")
                        value: "english-us"
                        selected: false
                    }
                }
                DropDown {
                    id: gender
                    objectName: "gender"
                    title: qsTr("Gender")
                    enabled: true
                    onSelectedOptionChanged: {
                        controller.saveSettings();
                    }
                    Option {
                        objectName: "female"
                        imageSource: "asset:///images/female.png"
                        text: qsTr("Female")
                        value: "female"
                        selected: true
                    }
                    Option {
                        objectName: "male"
                        imageSource: "asset:///images/male.png"
                        text: qsTr("Male")
                        value: "male"
                        selected: false
                    }
                }
                Container {
                    Label {
                        text: qsTr("Speed")
                    }
                }
                Container {
                    Slider {
                        objectName: "speed"
                        value: 1.0
                        fromValue: 0.2
                        toValue: 2.0
                        onValueChanged: {
                            controller.saveSettings();
                        }
                    }
                }
                Container {
                    Label {
                        text: qsTr("Pitch")
                    }
                }
                Container {
                    Slider {
                        objectName: "pitch"
                        value: 50
                        fromValue: 0
                        toValue: 100
                        onValueChanged: {
                            controller.saveSettings();
                        }
                    }
                }
            }
        }
    }
}
