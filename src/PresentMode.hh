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

#include <gazebo/msgs/any.pb.h>
#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <gazebo/gui/gui.hh>
#endif

namespace simslides
{
  class PresentModePrivate;

  /// \brief Dialog for creating a new presentation.
  class PresentMode : public QObject
  {
    Q_OBJECT

    /// \brief Constructor.
    /// \param[in] _parent Parent QWidget.
    public: PresentMode(QObject *_parent = 0);

    /// \brief Destructor.
    public: ~PresentMode();

    private slots: void OnToggled(bool _checked);
    private: void Start();
    private: void Stop();

    private: void OnKeyPress(ConstAnyPtr &_msg);
    private: void ChangeSlide();
    private slots: void OnSlideChanged(int);

    private: void OnWindowMode(const std::string &_mode);

    Q_SIGNALS: void SlideChanged(int, int);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<PresentModePrivate> dataPtr;
  };
  /// \}
}

#endif
