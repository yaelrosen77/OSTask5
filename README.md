# OSTask5
## Active Object (Actor) Design Pattern
Active Object is a Design Pattern in which Active Objects (Actors) are event-driven, strictly encapsulated software objects running
in their own threads of control that communicate with one another asynchronously by exchanging events. 

![image](https://github.com/yaelrosen77/OSTask5/assets/118935583/305292fc-3dca-4c6f-a42d-2fcff2ec5728) 
** Taken from _state-machine/Actors.com_ **

In this assignment we have been asked to deal with multiple threads in which communicate through a shared queue. 
It is build by: 
* Task- which is a runnable task (in our case a function that checks if numbers are prime) 
* A queue- one to support multiple threads 
* An active-object with the functionality of __creating a new active object__, a new queue of tasks in which each is instantiated, and will 
receive a pointer to a function that will execute each one in the queue, __a 'getQueue()' function__ 
