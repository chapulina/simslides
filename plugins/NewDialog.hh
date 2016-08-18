#ifndef SIMSLIDES_NEWDIALOG_HH_
#define SIMSLIDES_NEWDIALOG_HH_

#include <gazebo/gui/gui.hh>

namespace simslides
{
  class NewDialogPrivate;

  /// \addtogroup gazebo_gui
  /// \{

  /// \class NewDialog NewDialog.hh gui/gui.hh
  /// \brief Dialog for creating a new presentation.
  class NewDialog : public QDialog
  {
    Q_OBJECT

    /// \brief Constructor.
    /// \param[in] _mode Mode of the dialog.
    /// \param[in] _parent Parent QWidget.
    public: NewDialog(QWidget *_parent = 0);

    /// \brief Destructor.
    public: ~NewDialog();
    /// \internal
    /// \brief Pointer to private data.
    private: NewDialogPrivate *dataPtr;
  };
  /// \}
}

#endif