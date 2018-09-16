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
#ifndef SIMSLIDES_IMPORTDIALOG_HH_
#define SIMSLIDES_IMPORTDIALOG_HH_

#include <memory>

#include <gazebo/gui/qt.h>

namespace simslides
{
  class ImportDialogPrivate;

  /// \class ImportDialog ImportDialog.hh gui/gui.hh
  /// \brief Dialog for creating a new presentation.
  class ImportDialog : public QDialog
  {
    Q_OBJECT

    /// \brief Constructor.
    public: ImportDialog();

    /// \brief Destructor.
    public: ~ImportDialog();

    /// \brief Do the following:
    /// * Generate and save a .world file
    /// * Generate and save a model for each PNG, including material
    /// * Load keyframes
    /// * Load models
    private: void GenerateWorld();

    /// \brief Add <gui> to world SDF.
    /// \param[out] _worldSdf
    private: void AddGUI(std::string & _worldSdf);

    /// \brief Add all slide models to SDF.
    /// \param[out] _worldSdf
    private: void AddSlides(std::string & _worldSdf);

    /// \brief Callback to choose PDF file to be loaded.
    private slots: void OnBrowsePDF();

    /// \brief Callback to choose directory to save models.
    private slots: void OnBrowseDir();

    /// \brief Callback to load chosen PDF file.
    private slots: void OnLoadPDF();

    /// \brief Callback to go to next step after step 2.
    private slots: void OnNext2();

    /// \brief Callback to generate models.
    private slots: void OnGenerate();

    /// \brief Check if buttons should be enabled.
    private slots: void CheckReady(QString _str = QString());

    /// \brief Callback when conversion from PDF to images is completed.
    private slots: void OnConversionFinished(int, QProcess::ExitStatus);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<ImportDialogPrivate> dataPtr;
  };
}

#endif
