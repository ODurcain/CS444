I had a ludicrous amount of problems understanding the equations for standard deviation. I know how to do it as I've taken statistics, but implementing it into code was much more difficult than expected. It actually is what took the majority of my time I would say. Is was interesting to do command line argument parsing in this way as no other project has asked me to do this as such. There was the project 3 extra credit, but it was still slightly different. 

The main thing I enjoyed about this assignment was understanding how to work with the pthread library. This is definitely hitting the point of being extrememly complicated and being very difficult to learn, but that is what I would be hoping for from this final assignment. 

I haven't done rigorous testing on it, but I think the only bug I have found is sometimes the standard deviation for service time is sometimes -nan. I can not reliably replicate this, but it probably has something to do with my math. Other than that the only thing that I never got 100% content with was the queue length. I know this is a math conversion error, but it may also be how I'm storing the data in arrays. We'll call this a special feature rather than a bug.

Here's my sample output actually with the extra credit command line input!

./q -l 5 -m 2 -c 1000 -s 4

Mean Interarrival Time: 0.192548
Mean Waiting Time: 60.498930
Mean Service Time: 0.469185
Mean Queue Length: 5495782.000000
Standard Deviation Interarrival Time: 0.185312
Standard Deviation Waiting Time: 74.305849
Standard Deviation Service Time: 0.470431
Standard Deviation Queue Length: 77717725.851458
Server Utilization: 99.911200%

As you can see the main issue is queue length. Thanks for a fun semester! I learned a lot in this class about C and operating systems!