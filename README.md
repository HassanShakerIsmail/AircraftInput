# AircraftInput

This program begins by reading two seperate files, a CSV and JSON coresponding to data compiled by two aircraft sensors. The data of both sensors is formatted as follows, a sensor with a unique ID records the longtitude and latitude of a detected flying object.

The program then reads inputs from both files, and using the Haversine formula calculates the distance between pairs of flying objects detected by the sensors. If the distance between the two aerial objects found by the sensors is less than 100 (as found by the Haversine formula), the ID's of the two sensors are matched together.

That is, for example if sensor with ID 1 recorded an aerial object at say -50,50; and sensor with ID 2 detected an aerial object at say -49.99,49.99; the IDs 1 and 2 will be paired, and in an output file be denoted by '1:2'. 

If it is the case that a sensor cannot be paired with another from the other corresponding file, it will then by default be paired with '-1'.

This program is useful, as when comparing sensory data from multiple sensors, the program compares and outputs pairs quickly, allowing for things like determining errors on one of the sensors, whether a misread occured on one of the sensors, etc.

This program also does make a few assumptions. First and foremost, it is assumed that IDs in a file are unique. That is - in a particular JSON or CSV file, the ID say x, wouldn't appear more than once (It's totally okay if x appears in the other file though!)

Moreover, we do assume that the inputted data files are always in proper format, i.e CSV and JSON, with appropriate labels. No funny business!

This is admittedly a simple program, but I felt it appropriately demonstrated my familiarity with non standard libraries, particularly nlohmann's great JSON library. 
