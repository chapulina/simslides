#ifndef SIMSLIDES_LOADDIALOG_HH_
#define SIMSLIDES_LOADDIALOG_HH_

#include <memory>
#include "Common.hh"

#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <gazebo/gui/gui.hh>
#endif

namespace simslides
{
  class LoadDialogPrivate;

  /// \addtogroup gazebo_gui
  /// \{

  /// \class LoadDialog LoadDialog.hh gui/gui.hh
  /// \brief Dialog for creating a new presentation.
  class LoadDialog : public QDialog
  {
    Q_OBJECT

    /// \brief Constructor.
    /// \param[in] _mode Mode of the dialog.
    /// \param[in] _parent Parent QWidget.
    public: LoadDialog(QWidget *_parent = 0);

    /// \brief Destructor.
    public: ~LoadDialog();

    private slots: void OnBrowsePath();
    private slots: void Load();

    private slots: void CheckReady(QString _str = QString());

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<LoadDialogPrivate> dataPtr;
  };
  /// \}
}

#endif
