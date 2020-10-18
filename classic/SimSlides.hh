/*
 * Copyright 2016 Louise Poubel
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
*/
#ifndef SIMSLIDES_SIMSLIDES_HH_
#define SIMSLIDES_SIMSLIDES_HH_

#include <gazebo/gui/GuiPlugin.hh>
#include "PresentMode.hh"

namespace simslides
{
  class SimSlides : public gazebo::GUIPlugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: SimSlides();

    /// \brief Load SDF element.
    /// \param[in] _sdf SDF containing plugin configuration.
    public: void Load(const sdf::ElementPtr _sdf) override;

    /// \brief Callback when the user requests to change keyframes.
    /// \param[in] _keyframe Current keyframe number.
    /// \param[in] _total Total number of keyframes.
    private slots: void OnKeyframeChanged(const int _keyframe, const int _total);

    /// \brief Callback when the user requests to change text.
    /// \param[in] _text Text for dialog.
    private slots: void OnTextChanged(QString _text);

    /// \brief Callback when the keyframe number is changed. This is only called
    /// whent the widget loses focus or the user presses enter.
    private slots: void OnCurrentChanged();

    /// \brief Callback when the user starts presenting.
    private slots: void OnPresent();

    /// \brief Notifies that the keyframe spin has changed
    /// \param[in] _current Number of current keyframe.
    Q_SIGNALS: void CurrentChanged(const int _current);

    /// \brief Set total number of keyframes.
    /// \param[in] _total Total number of keyframes.
    Q_SIGNALS: void SetTotal(const QString _total);

    /// \brief Spin box holding current keyframe.
    private: QSpinBox * currentSpin{nullptr};

    /// \brief Holds text for keyframes.
    private: QTextBrowser * text{nullptr};

    /// \brief Present mode helper
    private: PresentMode * presentMode{nullptr};
  };
}
#endif
