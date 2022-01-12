# Reddit_IBM_Project
Using IBM Watson Natural Language Processing API along with Python to create a program that can process data points. The resulting returned data points contained keywords, sentiment score and emotion score from Watson's NLP.

# Basic Description of the Project and Workflow
The project's main goal was to clean data from a set of 2.1 million comments gathered from Reddit's r/Vive community and return the desired results from Waston NLP. The project was largely worked on using Git and GitHub for version control and much of the foundational code was already written when I came onto the project. All data collection was handled by the grad students we worked with and the final results were intended to provide data to be used in a paper analysing consumer trends and sentiment towards the HTC Vive VR headset.

# Workload
I was brought on to the project after the data collection had been completed. The files shown here is the sum total of the Python code used in this project to parse the data and determine the sentiment and mood of the text being processed. While I did not code all of these files (with the workload being shared equally amongst the 3 students in our group), I was expected to know how all of these files operated and to be able to debug and correct issues in the code.

The main issues with the code at the time were an incorrectly formatted output file (.csv) and general bugs that caused the program to error out after processing over 100 data points. I had to read through all of the created programs and find the issues that lead to errors in the output file and fix bugs with the code. I also ended up adding error logging and error handling to most of the code to troubleshoot the issues being experienced. 

After these initial problems were resolved, I also had to add features to better filter through useless data in the data set to be excluded from the final output. After this filtering was implemented, the final data count came out to be ~1.9 million comments and required multithreading to be processed in time for the paper submission deadline that our grad student partners had.

-------
NOTE: reddit_new.py and reddit_new_fixed_json_dump.py were the driving files for this project, with reddit_new.py outputing in .csv format and reddit_new_fixed_json_dump.py outputting in .json format.
