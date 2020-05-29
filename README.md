# Audio generation one-liners

This was a project in compute music course about audio generation one-liner: 

> You probably know of pseudo-random number generators that create long sequences of seemingly random numbers. 
A little thought should convince you that if the state of these algorithms is N bits, 
then the generated sequence cannot be longer than 2N. Thus, repetition will occur even if it takes eons. 
On the other hand, if the state is small, say N=20, then at audio rates of around 44,100 Hz, 
the maximum sequence length is 220/44100 = 23 seconds. 
Furthermore, really poor random-number generators can have internal periodicity so that the output 
does not act like random “noise.” Putting all this together, 
we can design really bad pseudo-random number generators that,
when turned into audio, contain interesting non-random structure that can be perceived as pitch and rhythm. 
The challenge then is to design “really bad” algorithms that sound as interesting as possible.

## To play with it
Download the unix executable one-liners and it should be ready to go. There are five lines and reverse is also possible. 

line_0: (t * (((t>>12) | (t>>8)) & (63 & (t>>4))))   
line_1: (t * ((t>>12 | t>>3) & 13131 & t>>11))   
line_2: ((440 * 256/8000 * t) & (1 << 7))   
line_3: (((t^0xf3c35)%64) * ((t/1537)&17) * sin(t/0x2500) + cos(t/0x2500) * (((t/3)^1984)%32) * ((t/90)&13))   
line_4: (((t+1<<(t>>t-(t*01231)>>43))/5)/(4.240101)-(t*0.00024))   
line_5: sin(t >> 2 | t << 8) * 50 * ((t>>2) & 3)   
