# ROSCon 2017

Using Gazebo 8

Tentative instructions:

~~~
cd ~/roscon2017
hg clone http://bitbucket.org/chapulina/simslides -b ROSCon_Sep2017
hg clone http://bitbucket.org/osrf/srcsim -b default
hg clone http://bitbucket.org/frank_b_mathis/val_gazebo_models/ -b default
cd simslides
sh build_plugins.sh
. ./roscon2017.sh
~~~














-----------------------------------------------------------------
-----------------------------------------------------------------
-----------------------------------------------------------------
-----------------------------------------------------------------

# Make sure you've installed Gazebo

http://gazebosim.org/tutorials?cat=install

Checking out a couple other tutorials wouldn't hurt either.

# Build and run SimSlides

1. If you have Gazebo 8 installed, run the following script to build simslides:

        sh build_plugins.sh

1. Run gazebo with the simslides plugin:

        . ./simslides.sh

# Your own presentation

You can generate your own presentation as follows:

## Generate a new presentation

1. On the top menu, choose Simslides -> Import PDF (or press F2)

1. Choose a PDF file from your computer

1. Choose the folder to save the generated models at

1. Choose a prefix for your model names, they will be named "prefix-0", "prefix-1", ...

1. Click Generate. A model will be created for each page of your PDF. This
   may take a while, the screen goes black... But it works in the end.
   Sometimes it looks like not all pages of the PDF become models... That's
   an open issue.

1. When it's done, all slides will show up on the world in a grid.

## Load previously generated slide models

If you already generated slide models with the step above, and now just
want to spawn them into the world:

1. On the top menu, choose Simslides -> Load Models (or press F3)

1. Choose the path of the directory which includes all the slides

1. Type in the model name prefix

1. Load

1. When it's done, all slides will show up on the world in a grid.

## Presentation model

Once you have the slides loaded into the world, present as follows:

1. Press F5 or the play button on the top left to start presentation mode

1. Press the arrow keys to go abck and forth on the slides

1. You're free to use the rest of Gazebo's interface while presenting,
   if you've navigated far away from the current slide, you can press F1
   to return to it.

1. At any moment, you can press F6 to return to the initial camera pose.

# Existing presentations

1. Move to the presentation branch, available ones are:

    * CppCon, September 2015

            hg up CppCon2015

    * University of Buenos Aires, November 2015

            hg up BuenosAires_Nov2015

    * Universidad de Chile, November 2015

            hg up Chile_Nov2015

    * IEEE Women in Engineering International Leadership Conference, May 2016

            hg up IEEE_WiE_ILC_2016

    * ROSCon, October 2016

            hg up ROSCon_Oct2016

    * ROS Industrial web meeting, January 2017

            hg up ROSIndustrial_Jan2017

    * Open Source Software for Decision Making, March 2017

            hg up OSS4DM_Mar2017

1. A lot changes from one presentation to the next, follow instructions on that
branch's README to run the presentation.


