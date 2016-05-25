# Make sure you've installed Gazebo

http://gazebosim.org/tutorials?cat=install

Checking out a couple other tutorials wouldn't hurt either.

# For general instructions, use the default branch and see the README there.

    hg up default


# Run the CppCon presentation

1. Compile plugins and prepare sides:

        sh prepare_all.sh

1. Source setup

        . setup.sh

1. Run final world (better than the auto generated one):

        gazebo final.world

