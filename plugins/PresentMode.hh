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

  /// \addtogroup gazebo_gui
  /// \{

  /// \class PresentMode PresentMode.hh gui/gui.hh
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
    private: void ChangeSlide(const int _key);

    Q_SIGNALS: void SlideChanged(int);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<PresentModePrivate> dataPtr;
  };
  /// \}
}

#endif
