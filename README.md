# Decrypt_Enigma
A terminal utility to decrypt Enigma-encrypted text. 

I've written several smaller programs to encrypt and attack text by several historical methods, but always wanted to try at Enigma. (I saw The Imitation Game when I was younger.) 
It turns out all I had to do was ask:  Cracking Enigma in 2021 - Computerphile  https://www.youtube.com/watch?v=RzWB5jL5RX0
I never would have come up with this on my own. Previous attacks I've used on mono-alphabetic substitutions were based on a vowel appearing every word, the frequency of every letter, the frequency of each letter starting a word, the frequency of which letters appeared in pairs, which letters were likely to be vowels based on the variance of letters they neighbored. This was all very complicated, but Index of Coincidence is extremely elegant. 

Thank you Mike Pound for making your code free to copy and review. I've passed that (Unlicense) along here as well. 
His code, the direct review of which made me ask (and answer) certain questions I otherwise would not have had:  https://github.com/mikepound/enigma
The original article he worked off of:  https://web.archive.org/web/20060720040135/http://members.fortunecity.com/jpeschel/gillog1.htm

I wrote this code with the intention of posting it, so I've tried to keep it clean, concise, commented. After finishing my C++ manual, I wondered what the criticisms of Object-Oriented Programming were on about ("everything is an object"). No offense, but reading this JavaScript made me understand those criticisms. Maybe it's just a language difference. After all, C++ does operate on low-level resources at the end of the day. I've organized this project by namespaces and header files, keeping helper functions out of classes, and keeping classes as concise as possible. 

This program can effectively recover the rotor settings. (A very impressive feat, again thanks to the above resources.) It struggles to find the plugboard settings. I believe this is because going one plug at a time doesn't produce better enough results on each iteration for the program to notice, especially when the rotors are already perfect. 
I must have rewatched the video ten times trying to figure out what he meant when he said that his end result was imperfect because they came out of the first stage with the ring settings zeroed out, thus supposedly throwing off later work. The algorithm (or at least my implementation) performs as advertised and can recover the precise rotor settings (except the irrelevant leftmost notch). I can only conclude that his failed to find the verbatim plaintext because of a failure to reconstruct the plugs, as mine does. 
(I also found this resource to finish the job on the plugboard, but frankly couldn't read it:  https://bibliotekanauki.pl/articles/305272.pdf  )

# How It Works

The only difference between us and our great-great-grandparents, is the amount of computations per second we can ask of our machines. We aren't going to do anything tricky or cunning.

The Enigma works like this: 
Keyboard Input >> Plugboard >> Rotors >> Reflector >> Rotors >> Plugboard >> Lightboard Output

The article that Mike based his code off of didn't talk about checking the reflector, so his code doesn't, and frankly that's why mine doesn't. 

We do have to check the Rotors and the Plugboard. We can do this one at a time, simplifying (Rotor Options)*(Plugboard Options) into (Rotor Options)+(Plugboard Options). 
The Rotors have to be chosen from the set I, II, ..., VIII. Then their rotations must be chosen. But there is an additional "ring" setting that affects the rotation of the next rotor. 
Fortunately we can also split off the ring setting into a separate step. 
So we get the "most English-like" Rotor choice and positions. Then the "most English-like" ring-setting (compensating that rotation by doing the opposite to the position). Then the "most English-like" Plugboard settings. 

Feel free to compile and run the code. All instructions should be provided upon use of "-h" as a CLI argument. 
But, "./decrypt_enigma --ciphertext ciphertext.txt" will run the program with default values on a text file called ciphertext.txt that is next to the program file. 
And, "./decrypt_enigma --encrypt --ciphertext ciphertext.txt --key IV U F III T K V D Z AZ GR HK" will decrypt that ciphertext using that key. 

