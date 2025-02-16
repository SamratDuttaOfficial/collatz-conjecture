import time

STARTING_SIEVE_K = 4
STARTING_2_POW_K = 2**STARTING_SIEVE_K
MAX_SIEVE_K = 16 # we will generate sieve bitset of size 2^16

print("current k:", STARTING_2_POW_K)

#current_sieve_pos = [3, 7, 11, 15, 19, 23, 35, 39, 43, 51, 55, 59, 
#67, 75, 79, 83, 87, 95, 99, 107, 115, 119, 123, 131, 135, 139, 
#143, 147, 151, 163, 171, 175, 179, 183, 187, 195, 199, 203, 211, 
#215, 219, 227, 235, 243, 247]
current_sieve_pos = [1, 3, 5, 9, 13, 17, 19]
current_sieve = [False] * (STARTING_2_POW_K)

def checkProof(num):
    alpha = next_2_pow_k
    beta = num%next_2_pow_k
    start_alpha = alpha
    start_beta = beta
    #steps = 0
    steps_log = []
    while (True):
        if (alpha <= start_alpha) and (beta<start_beta):
            return [True, steps_log]
        if alpha%2 != 0: break
        if beta%2 != 0:
            alpha = 3*alpha 
            beta = 3*beta + 1
        else:
            alpha = alpha/2
            beta = beta/2 
        #steps += 1
        #steps_log.append([alpha, beta])
    return [False, steps_log]

startTime = time.time() # start time of the algorithm

# populate current sieve
for i in range(STARTING_2_POW_K):
    if i in current_sieve_pos:
        current_sieve[i] = True 

totalRun = MAX_SIEVE_K - STARTING_SIEVE_K # total number of times this loop will be run
run = 0 # number of times this loop has been run
current_sieve_k = STARTING_SIEVE_K
current_2_pow_k = STARTING_2_POW_K
while (run < totalRun): 
    current_2_pow_k = 2**current_sieve_k
    next_sieve_k = current_sieve_k + 1
    next_2_pow_k = 2**next_sieve_k
    print("next k:", next_2_pow_k)

    next_sieve = [False]*(next_2_pow_k)

    # calculate next sieve 
    i = 3
    while (i < next_2_pow_k):
        if i%2 == 0: # even number
            next_sieve[i] = True
        j = i%current_2_pow_k
        if current_sieve[j]: # if (i mod 2^k) already exists as true in current sieve
            next_sieve[i] = True
        else:
            next_sieve[i] = checkProof(i)[0]
        i = i+1

    current_sieve = next_sieve
    current_sieve_k += 1
    run += 1

endTime = time.time() # end time of the algorithm

print(len(current_sieve))
print(endTime - startTime, "seconds")

trueCount = 0
# Write the bitset one bit at a time to the file
with open("bitset.txt", "w") as file:
    for bit in current_sieve:
        bit_value = "0"
        if bit:
            bit_value = "1"
            trueCount += 1
        file.write(bit_value)

print(trueCount)
sieve_size = pow(2,current_sieve_k)
perc = ((sieve_size - trueCount)/sieve_size)*100
print(perc, "% numbers need checking")