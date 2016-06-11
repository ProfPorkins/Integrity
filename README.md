# Integrity

The purpose of the code and book in this repository was a means for me to take some time to step back and rebuild some coding techniques and skill in advance of my long term goal of building a fully peer-to-peer massive virtual environment platform; no peer-to-peer code is contained in this repository.  The repository contains a book I have written that explains the C++11 and boost techniques I have used in writing the code.

At the time of this writing I'm not expecting to come back and make updates to the book or code in this repository.  The effort to create this book and code have accomplished my personal purpose, which is to help me build some updated skills and techniques.  The next step for me is to now take these techniques and begin my work on building the peer-to-peer platform.  I have some experience in doing this, as it was the focus of my PhD research, but needed to step back and restart with that long since completed.

## Book Description

The book begins with some basic C++11 techniques, moves to some basic boost asio networking samples, and progresses to a distributed, scalable, and fault-tolerant client-server framework.  Along the way a number of interesting topics are covered, such as scalability across multiple CPU cores using a task-based computing approach.  Networking using the Boost libraries is also covered, and then used to form the basis for the distributed code samples.

1. Introduction
2. Relevant C++11 Features
3. Scalability - Task-Based
4. Scalability - Priority
5. Scalability - Task Dependencies
6. Networking Techniques
7. Message Coding
8. Scalability - Distributed
9. Scalability - Fault-Tolerant - Task Dependencies

## Sample Code Notes

There are two primary folders that contain code, named *Book-Code* and *Book-Code2*.  The samples in *Book-Code* are those presented in the book text.  The samples in *Book-Code2* are modified from the samples presented in the book to use Google Protocol Buffers for the message serialization rather than the custom byte encoding scheme presented in the book.  I admit it would be nice if I updated the book text to provide a discussion of this, but I'm doubtful I'll take the time to do that.