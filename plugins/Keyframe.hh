#ifndef SIMSLIDES_KEYFRAME_HH_
#define SIMSLIDES_KEYFRAME_HH_

#include <memory>
#include <sdf/sdf.hh>
#include <ignition/math/Pose3.hh>

#include <gazebo/gui/gui.hh>

namespace simslides
{
  class KeyframePrivate;

  /// \brief Keyframe types
  enum KeyframeType
  {
    LOOKAT,
    STACK,
    LOG_SEEK
  };

  class Keyframe
  {
    /// \brief Constructor.
    /// \param[in] _sdf
    public: Keyframe(sdf::ElementPtr _sdf);

    /// \brief Destructor.
    public: ~Keyframe();

    /// \brief Get the full type definition.
    /// \return The full type definition.
    public: KeyframeType GetType() const;

    /// \brief
    /// \return
    public: unsigned int SlideNumber() const;

    /// \brief For LOG_SEEK
    /// \return
    public: ignition::math::Pose3d CamPose() const;

    /// \brief For LOOKAT
    /// \return
    public: ignition::math::Pose3d EyeOffset() const;

    /// \brief
    /// \return
    public: gazebo::common::Time LogSeek() const;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<KeyframePrivate> dataPtr;
  };
  /// \}
}

#endif
