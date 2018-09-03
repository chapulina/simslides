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
