# Make sure you've installed Gazebo

http://gazebosim.org/tutorials?cat=install

Checking out a couple other tutorials wouldn't hurt either.


# Prepare all

1. Save your slides as "slides/slides.pdf"

1. Run:

    sh prepare_all.sh

1. The script:

    * Compiles all plugins

    * Generates a simple world from a PDF file with a camera poses plugin

# Update environment variables

Source setup (currently tailored for a specific machine, you'll need to edit the script with the path in your computer):

    . setup.sh

# Run word:

    gazebo worlds/slides.world



