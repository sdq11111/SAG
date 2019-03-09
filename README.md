# SAG
This is the source code of SAG, which is used to extract the video tags from time-sync comments by a semantic associate graph (SAG). This program can be run by mingw32-g++ compiler under Windows. We provide sample data with 239 time-sync comments for debugging.

# Input File
sentence_vector: The folder contains the sentence vector of time-sync comments. The suffix of the file in sentence_vector should be ".txt". In the sample data, the "sample.txt" contains 2*239 lines. Each time-sync comment in sample data contains two lines. The first line contains the timestamp and a time-sync comment. And the second line contains the 200-dimensional sentence vector.
word_segmentation: The folder contains the participle results of time-sync comments with part of speech.\

segFile: the name of the folder which is used to store the participle results of time-sync comments with part of speech.

#
