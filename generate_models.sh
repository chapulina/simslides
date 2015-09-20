#!/bin/bash

echo "Preparing to generate PNGs..."
# remove current world folder
rm -rf world
mkdir world
cd world

echo "Generating PNGs from slides..."
# generate into temp folder
mkdir temp
convert ../slides/slides.pdf temp/slides.png

# substitute dash with underscore
cd temp
for i in `ls *-*`; do
  NEW=`echo $i|tr '-' '_'`
  mv $i $NEW
done

# start world SDF
  echo "
<?xml version='1.0' ?>
<sdf version='1.5'>
  <world name='default'>
    <scene>
      <grid>0</grid>
      <origin_visual>0</origin_visual>
      <sky>
        <clouds>
          <speed>20</speed>
          <humidity>5</humidity>
        </clouds>
      </sky>
    </scene>

    <!-- A global light source -->
    <include>
      <uri>model://sun</uri>
    </include>

    <!-- A ground plane -->
    <include>
      <uri>model://ground_plane</uri>
    </include>

    <!-- Quadcopter -->
    <model name='quadrotor'>
      <link name='link'>
	<pose>0 0 0.182466 0 0 0</pose>
	<inertial>
	  <mass>1.316</mass>
	  <inertia>
	    <ixx>0.0128</ixx>
	    <ixy>0</ixy>
	    <ixz>0</ixz>
	    <iyy>0.0128</iyy>
	    <iyz>0</iyz>
	    <izz>0.0218</izz>
	  </inertia>
	</inertial>
	<collision name='collision'>
	  <geometry>
	    <mesh>
	      <uri>model://quadrotor/meshes/quadrotor_base.dae</uri>
	    </mesh>
	  </geometry>
	</collision>
	<visual name='visual'>
	  <geometry>
	    <mesh>
	      <uri>model://quadrotor/meshes/quadrotor_base.dae</uri>
	    </mesh>
	  </geometry>
	</visual>
      </link>
      <plugin name='animated_quad' filename='libanimated_quad.so'>
        <pose>0 0 4.5 0 0 0</pose>
        <pose>30 0 4.5 0 0 0</pose>
        <pose>30 30 4.5 0 0 0</pose>
        <pose>0 30 4.5 0 0 0</pose>
        <pose>0 0 4.5 0 0 0</pose>
      </plugin>
    </model>

    " >> ../slides.world

echo "Creating a model for each slide..."
mkdir ../models
countX=0
countY=0
totalCount=0
for image in `ls *.png | sort -V`; do
  # extract name
  filename=$(basename "$image")
  filename="${filename%.*}"
  echo "Processing $filename ...";

  # create model folder structure
  mkdir ../models/$filename
  mkdir ../models/$filename/materials
  mkdir ../models/$filename/materials/scripts
  mkdir ../models/$filename/materials/textures

  # move image to textures
  mv $image ../models/$filename/materials/textures/$image

  # create material script
  echo "
material Slides/"$filename"
{
  receive_shadows off
  technique
  {
    pass
    {
      texture_unit
      {
	texture "$image"
	filtering anistropic
	max_anisotropy 16
      }

    }
  }
}
    " >> ../models/$filename/materials/scripts/script.material

  # create config file
  echo "
<?xml version='1.0'?>
<model>
  <name>"$filename"</name>
  <version>1.0</version>
  <sdf version='1.5'>model.sdf</sdf>

  <description>
    Slide
  </description>
</model>
    " >> ../models/$filename/model.config

  # create model SDF file
  echo "
<?xml version='1.0' ?>
<sdf version='1.5'>
  <model name='"$filename"'>
    <static>true</static>
    <link name='link'>
      <pose>0 0 1.5 0 0 0</pose>
      <collision name='collision'>
        <geometry>
          <box>
            <size>4.0 1.0 3.0</size>
          </box>
        </geometry>
      </collision>
      <visual name='visual'>
        <geometry>
          <box>
            <size>4.0 1.0 3.0</size>
          </box>
        </geometry>
        <material>
          <script>
            <uri>model://"$filename"/materials/scripts</uri>
            <uri>model://"$filename"/materials/textures</uri>
            <name>Slides/"$filename"</name>
          </script>
        </material>
      </visual>
    </link>
  </model>
</sdf>
    " >> ../models/$filename/model.sdf

  # include in world SDF
  echo "
    <include>
      <name>"$filename"</name>
      <pose>"$countX" "$countY" 0 0 0 0</pose>
      <uri>model://"$filename"</uri>
    </include>
    " >> ../slides.world

  if [ "$countX" -gt "30" ]; then
    countX=0
    countY=$((countY + 10))
  else
    countX=$((countX + 10))
  fi
  totalCount=$((totalCount + 1))

done

# add camera pose plugin with poses matching ones above
echo "
    <gui>
      <plugin name="camera_poses" filename="libcamera_poses_plugin.so">
  " >> ../slides.world

countX=0
countY=0
counter=0
while [ $counter -lt $totalCount ]; do

  echo "
      <pose>"$((countX))" "$((countY - 6))" 3 0 0.25 1.57</pose>
    " >> ../slides.world

  if [ "$countX" -gt "30" ]; then
    countX=0
    countY=$((countY + 10))
  else
    countX=$((countX + 10))
  fi
  counter=$((counter + 1))

done

echo "
      </plugin>
    </gui>
  </world>
</sdf>
  " >> ../slides.world
