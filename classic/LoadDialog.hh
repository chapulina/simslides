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
#ifndef SIMSLIDES_LOADDIALOG_HH_
#define SIMSLIDES_LOADDIALOG_HH_

#include <memory>

#include <gazebo/gui/qt.h>

namespace simslides
{
  class LoadDialogPrivate;

  /// \brief Dialog to load previously generated slides.
  class LoadDialog : public QDialog
  {
    Q_OBJECT

    /// \brief Constructor.
    public: LoadDialog();

    /// \brief Destructor.
    public: ~LoadDialog();

    /// \brief Callback when browse button is clicked.
    private slots: void OnBrowsePath();

    /// \brief Callback when load button is clicked.
    private slots: void Load();

    /// \brief Update state in case it's ready to present.
    /// \param[in] _str Unused
    private slots: void CheckReady(QString _str = QString());

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<LoadDialogPrivate> dataPtr;
  };
}

#endif
