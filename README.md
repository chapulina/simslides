# Make sure you've installed Gazebo

http://gazebosim.org/tutorials?cat=install

Checking out a couple other tutorials wouldn't hurt either.

# Prepare presentation

You can generate your own presentation or run one of the existing ones.

## Generate your own presentation

1. Run gazebo with the simslides plugin:

        . ./simslides.sh


1. Generate models for your slides

    1. On the top menu, choose Simslides -> Import PDFi (or press F2)

    1. Choose a PDF file from your computer

    1. Choose the folder to save the generated models at

    1. Choose a prefix for your model names, they will be named "prefix-0", "prefix-1", ...

    1. Click Generate. A model will be created for each page of your PDF. This
       may take a while, the screen goes black... But it works in the end.
       Sometimes it looks like not all pages of the PDF become models... That's
       an open issue.

    1. When it's done, all slides will show up on the world in a grid.

1. Load existing slide models

    If you already generated slide models with the step above, and now just
    want to spawn them into the world:

    1. On the top menu, choose Simslides -> Load Models (or press F3)

    1. Choose the path of the directory which includes all the slides

    1. Type in the model name prefix

    1. Load

    1. When it's done, all slides will show up on the world in a grid.


## Existing presentations

1. Move to the presentation branch, available ones are:

    * CppCon, September 2015

            hg up CppCon2015

    * University of Buenos Aires, November 2015

            hg up BuenosAires_Nov2015

    * Universidad de Chile, November 2015

            hg up Chile_Nov2015

    * IEEE Women in Engineering International Leadership Conference, May 2016

            hg up IEEE_WiE_ILC_2016

1. Build plugins:

        sh build_plugins.sh

# Run presentation

To run the roscon presentation:

1. In one terminal, run:

    . ./roscon.sh

1. On another terminal, run:

    . ./playback.sh

