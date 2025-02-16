# Chronological Verification on the Collatz Conjecture using Theoretically Proven Sieves

Research paper Pre-print: https://www.researchgate.net/publication/386290188_Chronological_Verification_of_the_Collatz_Conjecture_using_Theoretically_Proven_Sieves

### Steps:

Generate the sieve bitset:

```
python generate-sieve.py
```

Edit the `MAX_SIEVE_K` according to your needs in the python program to generate a bitset of size `2^(MAX_SIEVE_K)`.

##### Warning

Anything bigger than 32 will consume a humongous amount of memory and disk space! 

Run the collatz conjecture verification:

```
g++ main.cpp -o main # compile the program
main.exe # run it
```

Edit the `MAX_POW_TEN` parameter in the C++ program to determine how many numbers to check in a single core of your CPU in a single run. The program will check `10^(MAX_POW_TEN)` numbers in a single core.

Edit the `START_POW_TWO` parameter to determine which number ti start checking from. The program will start checking from `2^(START_POW_TWO)` number. 

##### Warning:

`MAX_POW_TEN` greater than 11 will consume a tremendous amount of time!

`START_POW_TWO` higher than 120 might cause program failure as the max integer it can handle is `(2^128 -1)` (we use uint128_t for integers). You might need to make changes in the program for bigger numbers. 

### Conclusion

This is an old version of the program. I lost some changes switching from my windows machine to mac. Lt me know if there are some mistakes in the program.

Pull requests are welcome.