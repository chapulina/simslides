# Make sure you've installed Gazebo

http://gazebosim.org/tutorials?cat=install

Checking out a couple other tutorials wouldn't hurt either.

# Running existing presentations

## CppCon 2015

1. Go to the branch

    hg up CppCon2015

1. Source setup (currently tailored for a specific machine, you'll need to edit the script with the path in your computer):

    . setup.sh

1. Run the word, which has been manually modified:

    gazebo final.world

## BuenosAires_Nov2015

1. Go to the branch

    hg up BuenosAires_Nov2015

1. Source setup (currently tailored for a specific machine, you'll need to edit the script with the path in your computer):

    . setup.sh

1. Run the word, which has been manually modified:

    gazebo final.world

# Make your own presentation

1. Save your slides as "slides/slides.pdf"

1. Run:

    sh prepare_all.sh

1. The script:

    * Compiles all plugins

    * Generates a simple world from a PDF file with a camera poses plugin

1. Update environment variables: source setup (currently tailored for a specific machine, you'll need to edit the script with the path in your computer):

    . setup.sh

1. Run word:

    gazebo worlds/slides.world

# Manual tweaks

1. Copy the auto generated world to the main folder

    cp worlds/slides.world final.world

1. Modify final.world and run that