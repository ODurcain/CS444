Owen Durkin

This assignment had a lot of external factors playing into the incompleteness of it. Firstly, there was a steep learning curve for C as the teaching for C at UMB is only one class and this is the only other class I've had in which it is used. Once I got my bearings with C, I didn't struggle as much as I thought I would have. I was able to fairly easily understand how to obtain frequencies and the ascii representations of the characters. I even managed to add all of the starting frequencies for completeshakespeare.txt together correctly. The real struggle that I began to face was when compressing all of the ascii values together I was getting a very small kb of only about 3,xxx instead of the supposed output of 3,xxx,xxx kb.

At this point all of my frequencies, the binary representation of said characters, and nodes appear to be set up correctly. It is just the actual compression that is failing.

There was a lot of trouble shooting and there was so much documentation for this algorithm and so many different approaches that I found a logic that made sense to me and began to implement it. Long story short there are a lot of function calls and it could possibly be made more efficient, but I'm pleased with my end result. 

I have many structs that I reference outside of the main function for things like creating nodes, the minheap, the Huffman conversion to binary, the character frequencies. The node struct is arguably the most used one throughout my whole program as many functions rely on it.

The logic was mainly first to get the frequencies and then convert the ascii characters to their binary representation which is relative to their frequencies. The node closest to root was the ascii character for space which had the binary of 10 and a frequency of 1240811 and the furthest one was / and binary of 110100001101101001 and a frequency of 12. Was neat to see the difference between the highest and lowest frequencies. 

My biggest problem was actually with my encoding and writing to the output file. I was originally having the problem I described earlier of my output file growing massive due to some strange recursion error happening in the write action that I still don’t fully understand. Then I was printing nothing to the output file which is the polar opposite problem. Eventually I tried putting the encode function inside of my main function instead of calling it separately. This fixed everything I had wrong and my output file was perfect.

Overall this was a great assignment and I feel it really helped me understand C. The problems that I faced were more so funny than anything as creating an infinitely large output file that was 100x bigger than the input file in a algorithm that is mean to compress file size is hilariously ironic.
