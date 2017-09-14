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
    LOOKAT            = 0x00000001,
    STACK             = 0x00000002,
    CAM_POSE          = 0x00000004,
  };

  class Keyframe
  {
    /// \brief Constructor.
    /// \param[in] _sdf
    public: Keyframe(sdf::ElementPtr _sdf);

    /// \brief Destructor.
    public: ~Keyframe();

    /// \brief Add a type specifier.
    /// \param[in] _type New type to append to this keyframes type
    /// definition.
    public: void AddType(KeyframeType _type);

    /// \brief Returns true if this keyframe's type definition has the
    /// given type.
    /// \param[in] _t Type to check.
    /// \return True if this keyframe's type definition has the.
    public: bool HasType(const KeyframeType &_t) const;

    /// \brief Get the full type definition.
    /// \return The full type definition.
    public: unsigned int GetType() const;

    /// \brief
    /// \return
    public: unsigned int SlideNumber() const;

    /// \brief
    /// \return
    public: ignition::math::Pose3d CamPose() const;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<KeyframePrivate> dataPtr;
  };
  /// \}
}

#endif
