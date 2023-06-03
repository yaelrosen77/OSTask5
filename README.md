# OSTask5
## Active Object (Actor) Design Pattern
Active Object is a Design Pattern in which Active Objects (Actors) are event-driven, strictly encapsulated software objects running
in their own threads of control that communicate with one another asynchronously by exchanging events. 

![image](https://github.com/yaelrosen77/OSTask5/assets/118935583/305292fc-3dca-4c6f-a42d-2fcff2ec5728) 
** Taken from _state-machine/Actors.com_ **
## Building pipe-line 
In this assignment we have been asked to deal with multiple threads in which communicate through a shared queue. 
It is build by: 
* Task- which is a runnable task (in our case a function that checks if numbers are prime) 
* A queue- one to support multiple threads 
* An active-object with the functionality of __creating a new active object__, a new queue of tasks in which each is instantiated, and will 
receive a pointer to a function that will execute each one in the queue, __a 'getQueue()' function__ that will return a pointer to a queue of 
an active object argument, and a __stop() function__ to shut down the active-object given in the parameters and also free all its memory related.  
* a Pipe-Line that gets a number of tasks N to be performed, and a _randmseed_ for raffling a number to work with. The first AO will generate N six-digit
numbers. Than by small periods will pass all of the numbers one by one to the next AO, where its jobs is now to print the number, check if it is prime, change it and 
pass to the next AO and so on.. After running N numbers and perform the tests and changes, the program ends. 

## Running
After making sure you have the write compiler configurations, compile with the cmd <make all> on linux terminal. 
You will get a c compiled program in which you can run with the command - ./st_pipeline <N> <K> - where N is the number of tasks/runs and k is the random seed which by default be set by cur time header file.

Good luck:)
