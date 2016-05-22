# Make sure you've installed Gazebo

http://gazebosim.org/tutorials?cat=install

Checking out a couple other tutorials wouldn't hurt either.

# Prepare presentation

You can generate your own presentation or run one of the existing ones.

## Generate your own presentation

1. From this directory, save your slides at `slides/slides.pdf`

1. Run:

        sh prepare_all.sh

    The script:

        * Compiles all plugins (`build_plugins.sh`)

        * Generates a simple world from a PDF file with a camera poses plugin (`generate_models.sh`)

1. Manual tweaks

    1. Copy the auto generated world to the main folder

            cp world/slides.world final.world

    1. It's possible to "stack" slides to make more interesting transitions. Edit the `stack` file with
       "<slide number> <stack level>" on each line. Stack levels:

        * none: Slide not on stack
        * o: Slide is the start of a stack
        * 1: Slide is in the middle of a stack
        * 2: Slide is the end of a stack

    1. Modify `final.world` using `gzclient` (move slides around, insert models, etc.) and save it.


## Existing presentations

1. Move to the presentation branch, available ones are:

    * CppCon 2015

            hg up CppCon2015

    * University of Buenos Aires, November 2015

            hg up BuenosAires_Nov2015

1. Build plugins:

        sh build_plugins.sh

# Run presentation

1. Make sure you've loaded the quadrotor model

        wget https://bitbucket.org/osrf/gazebo_models/raw/default/quadrotor/model.sdf ~/.gazebo/models/quadrotor/model.sdf
        wget https://bitbucket.org/osrf/gazebo_models/raw/default/quadrotor/model.config ~/.gazebo/models/quadrotor/model.config

1. Source setup:

        . setup.sh

1. Run the final word:

        gazebo final.world

