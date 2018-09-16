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
#ifndef SIMSLIDES_PRESENTMODE_HH_
#define SIMSLIDES_PRESENTMODE_HH_

#include <memory>

#include <gazebo/gui/gui.hh>
#include <gazebo/msgs/any.pb.h>

namespace simslides
{
  class PresentModePrivate;

  /// \brief Handles keyframes while presenting.
  class PresentMode : public QObject
  {
    Q_OBJECT

    /// \brief Constructor.
    public: PresentMode();

    /// \brief Destructor.
    public: ~PresentMode();

    /// \brief Callback when user starts / stops present mode.
    /// \param[in] _checked True to run, false to stop.
    private slots: void OnToggled(const bool _checked);

    /// \brief Start presenting.
    private: void Start();

    /// \brief Stop presenting.
    private: void Stop();

    /// \brief Callback when user presses a key.
    /// \param[in] _msg Message containing key.
    private: void OnKeyPress(ConstAnyPtr &_msg);

    /// \brief Callback when Gazebo says the window mode has changed.
    /// \param[in] _mode New mode, usually "simulation" or "LogPlayback".
    private: void OnWindowMode(const std::string &_mode);

    /// \brief Performs the slide change, based on the current index which was
    /// previously set.
    private: void ChangeSlide();

    /// \brief Callback when the user requested a new keyframe.
    /// \oaram[in] _slide New slide index.
    private slots: void OnSlideChanged(int _slide);

    /// \brief Notifies that the slide index has changed,
    /// \param[in] _currentIndex Current keyframe index.
    /// \param[in] _slideCount Total number of keyframes.
    signals: void SlideChanged(int _currentIndex, int _slideCount);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<PresentModePrivate> dataPtr;
  };
  /// \}
}

#endif
