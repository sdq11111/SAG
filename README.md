# SAG
This is the source code of SAG, which is used to extract the video tags from time-sync comments by a semantic associate graph (SAG). This program can be run by mingw32-g++ compiler under Windows. We provide sample data with 239 time-sync comments for debugging.

# Input File
sentence_vector: The folder contains the sentence vector of time-sync comments. The suffix of the file in sentence_vector should be ".txt". In the sample data, the "sample.txt" contains 2*239 lines. Each time-sync comment in sample data contains two lines. The first line contains the timestamp and a time-sync comment. And the second line contains the 200-dimensional sentence vector. In this paper, 

word_segmentation: The folder contains the participle results of time-sync comments with part of speech. The suffix of the file in sentence_vector should be ".txt". The file name in “word_segmentation” must be same in "sentence_vector". In the sample data, the "sample.txt" contains 239 lines. Each time-sync comment in sample data contains one line: Chinese word segmentation results (including part of speech).

idf2.txt: The word IDF scores.

# Output File
result: The folder contains the results of video tag extraction.

# program
dialogue.cpp: The model with dialogue-based cluster algorithm. 

topic_center.cpp: The model with topic center-based cluster algorithm.

A detailed explanation of the programs can be found in the program annotation.

# Notice
This program only supports running under the Windows operating system. The full data set for the experiment could not be published due to copyright reasons. You can email sdq11111@sjtu.edu.cn to request part of the data.
