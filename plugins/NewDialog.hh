#ifndef SIMSLIDES_NEWDIALOG_HH_
#define SIMSLIDES_NEWDIALOG_HH_

#include <memory>

#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <gazebo/gui/gui.hh>
#endif

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

    /// \brief Qt callback when the file directory browse button is pressed.
    private slots: void OnBrowsePDF();
    private slots: void OnBrowseDir();
    private slots: void OnGenerate();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<NewDialogPrivate> dataPtr;
  };
  /// \}
}

#endif
