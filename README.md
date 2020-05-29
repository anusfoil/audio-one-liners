# Audio generation one-liners

This was a project in compute music course about audio generation one-liner: 

You probably know of pseudo-random number generators that create long sequences of seemingly random numbers. 
A little thought should convince you that if the state of these algorithms is N bits, 
then the generated sequence cannot be longer than 2N. Thus, repetition will occur even if it takes eons. 
On the other hand, if the state is small, say N=20, then at audio rates of around 44,100 Hz, 
the maximum sequence length is 220/44100 = 23 seconds. 
Furthermore, really poor random-number generators can have internal periodicity so that the output 
does not act like random “noise.” Putting all this together, 
we can design really bad pseudo-random number generators that,
when turned into audio, contain interesting non-random structure that can be perceived as pitch and rhythm. 
The challenge then is to design “really bad” algorithms that sound as interesting as possible.
