CardChecker
-----------
by slod

A program to find cards with duplicate character data.

Use   - drag a folder onto the program, it will then check that folder and all sub-folders for 
        character cards, get a check sum for the character data in each card, then search for
        duplicates.

Version History
---------------
v0.1    - development version
v1.0    - initial release
v1.1    - solving the waifu problem


Notes
-----

The details of any duplicates it finds will be stored in a file called "CardChecker results.txt"
    that should be created in the folder containing the folder you dragged to the program.
    (e.g. if you dragged the "chara" folder from "UserData" then the results go into "UserData".)

A log file containing any abnormalities (such as normal .png files disguised as cards) and other
    errors will be created in the same place

This program now detects duplicate cards created by Waifu Ripper as well as card duplicates
    created in the charater editor by saving the same character but using different settings
    when creating the card image. (such as a different pose, or wearing a different one of their
    3 outfits -- these options are actually saved in the character section of the card.)
