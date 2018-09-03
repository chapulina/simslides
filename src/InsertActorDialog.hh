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
#ifndef SIMSLIDES_INSERTACTORDIALOG_HH_
#define SIMSLIDES_INSERTACTORDIALOG_HH_

#include <memory>

#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <gazebo/gui/gui.hh>
#endif

namespace simslides
{
  class InsertActorDialogPrivate;

  /// \addtogroup gazebo_gui
  /// \{

  /// \class InsertActorDialog InsertActorDialog.hh gui/gui.hh
  /// \brief Dialog for creating a new presentation.
  class InsertActorDialog : public QDialog
  {
    Q_OBJECT

    /// \brief Constructor.
    /// \param[in] _mode Mode of the dialog.
    /// \param[in] _parent Parent QWidget.
    public: InsertActorDialog(QWidget *_parent = 0);

    /// \brief Destructor.
    public: ~InsertActorDialog();

    /// \brief Qt callback when the file directory browse button is pressed.
    private slots: void OnBrowseSDF();
    private slots: void OnInsert();
    private slots: void CheckReady(QString _str = QString());

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<InsertActorDialogPrivate> dataPtr;
  };
  /// \}
}

#endif
