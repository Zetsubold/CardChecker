CardChecker
-----------
by slod

A program to find cards with duplicate character data.

Use   - drag a folder onto the program, it will then check that folder and all sub-folders for 
        character cards, get a check sum of the character data in each card, then search for
         duplicates.

Version History
---------------
v0.1    - development version
v1.0    - initial release


Notes
-----

The details of any duplicates it finds will be stored in a file called "CardChecker results.txt"
    that should be created in the folder containing the folder you dragged to the program.
    (e.g. if you dragged the "chara" folder from "UserData" then the results go into "UserData".)

A log file containing any abnormalities (such as normal .png files disguised as cards) and other
    errors will be created in the same place

My testing has revealed that cards created by Waifu Ripper from scenes with multiple copies of
    the same character don't contain exactly the same character data in each card.  When I work
    out how to detect this I will release an updated version.