#ifndef SIMSLIDES_IMPORTDIALOG_HH_
#define SIMSLIDES_IMPORTDIALOG_HH_

#include <memory>

#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <gazebo/gui/gui.hh>
#endif

namespace simslides
{
  class ImportDialogPrivate;

  /// \addtogroup gazebo_gui
  /// \{

  /// \class ImportDialog ImportDialog.hh gui/gui.hh
  /// \brief Dialog for creating a new presentation.
  class ImportDialog : public QDialog
  {
    Q_OBJECT

    /// \brief Constructor.
    /// \param[in] _mode Mode of the dialog.
    /// \param[in] _parent Parent QWidget.
    public: ImportDialog(QWidget *_parent = 0);

    /// \brief Destructor.
    public: ~ImportDialog();

    /// \brief Qt callback when the file directory browse button is pressed.
    private slots: void OnBrowsePDF();
    private slots: void OnBrowseDir();
    private slots: void OnGenerate();
    private slots: void CheckReady(QString _str = QString());

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<ImportDialogPrivate> dataPtr;
  };
  /// \}
}

#endif
