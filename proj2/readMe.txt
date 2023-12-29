Owen Durkin

I started by first creating a program that could read the files. Then setting up the files to be outputs 
of the desired bits. This was fairly straightforward as most of this had no encoding logic yet. I then broke the file into 4 byte blocks.
the purpose of the was to convert the data into a 8-bit string to be used for futher manipulation. I took each bit of the string and 
converted it to binary. I then used a simple xor comparison to get all of the necessary hamming(7,4) codes. Next was the tricky part.
I started by initially just writing the binary values bit by bit to the file. which was obviously wrong, so I went back and converted them 
to hex, but for some reason when I wrote them to thie file the output files were 3GB. I think I restarted my raid.c about 5-6 times.
I was increibly surprised with how difficult this was.

I'm honestly just writing notes in this as I go through and encounter things. My raid.c works beautifully,
but my problem lies in the memory allocation side of things. I have very little experience with this and it's causing me problems 
with understanding how to do this correctly. I need an adjustable 2d array, but every time I try to set this up I begin having a similar problem
to what I was seeing in project 1 where the files would get infinitely large.

At the moment I'm going to continue to base everything off of the test.txt file working correctly and start diar.c from there. I will figure out 
dynamic memory allocation at a later date. I don't think I know quite enough how to make the dynamic memory allocation work. 
I have a version called raidnope.c that actually does write complete shakespeare correctly into parts, but it is binary.

