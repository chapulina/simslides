/*
 * Copyright (C) 2020 Louise Poubel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RowLayout {
  id: simSlides
  Layout.minimumWidth: 250
  Layout.minimumHeight: 100
  spacing: 2

  property int lastKeyframe: 10;

  Connections {
    target: SimSlidesIgn
    onUpdateGUI: {
      keyframeSpin.value = _currentKeyframe;
      simSlides.lastKeyframe = _keyframeCount;
    }
  }

  SpinBox {
    id: keyframeSpin
    from: 0
    to: simSlides.lastKeyframe
    editable: true
    ToolTip.text: "Current slide"
    ToolTip.visible: hovered
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    onValueModified: {
      SimSlidesIgn.OnKeyframeChanged(keyframeSpin.value);
    }
  }
}
