#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <cstring>

using namespace std::chrono;

#define uint128_t __uint128_t

#define MAX_POW_TEN 9 // in one core 10^9 numbers will be checked
#define START_POW_TWO 100 // we start checking numbers from 2^START_POW_TWO (2^5 for example)

// if checkNNumbers is checking N number of integers, it will
// be broken into alpha number of loops instead of a single loop.
// each inner loop will be checking N/alpha number of integers.
// if N/alpha is not a multiple of 4, alpha may cause extra 
// checks in each core, causing duplicate checks. 
const int alpha = 1; 

// integers to skip among 0->256 integers (except even numbers)
const int skip[] = {3, 7, 11, 15, 19, 23, 35, 39, 43, 51, 55, 59, 
67, 75, 79, 83, 87, 95, 99, 107, 115, 119, 123, 131, 135, 139, 
143, 147, 151, 163, 171, 175, 179, 183, 187, 195, 199, 203, 211, 
215, 219, 227, 235, 243, 247};
// number of elements in the skip array
const int skipLength = 45;
// the length of the sieve bitset will be 2^sieve_k 
// this sieve_k is different from another var k used below
const int sieve_k = 16;
// below we perform left shift to calculate 2^sieve_k
// 1 is an int. we make it unsigned long long (64 bit) with ULL.
const uint64_t sieve_length = 1ULL << sieve_k; 
// sieve bitset is a bitset with i-th value as true if i-th integer 
// is to be skipped checking among 0->2^k integers. those will converge
// the bitset is populated with false by default
bool* sieve = new bool[1ULL << sieve_k]; // 1<<n means 2^n
// bool sieve[1 << sieve_k] = {false};

void print_int128(uint128_t num) {
    char str[40];
    int i = 0, negative = 0;
    if (num < 0) {
        negative = 1;
        num = -num;
    }
    while (num > 0) {
        str[i++] = num % 10 + '0';
        num /= 10;
    }
    if (i == 0) {
        std::cout << "0";
    } else {
        if (negative) {
            std::cout << "-";
        }
        while (i > 0) {
            std::cout << str[--i];
        }
    }
}

uint128_t pow2(int n) {
    uint128_t result = 1;
    uint128_t base = 2;
    while (n > 0) {
        if (n % 2 == 1) {
            result *= base;
        }
        base *= base;
        n /= 2;
    }
    return result;
}

uint128_t getPower(uint128_t base, uint32_t n) {
    uint128_t result = 1;
    for (uint32_t i = 0; i < n; ++i) {
        result *= base;
    }
    return result;
}

uint32_t count_trailing_zeros(uint32_t n) {
    uint32_t count = 0;
    while ((n & 1) == 0 && n != 0) {
        count++;
        n >>= 1;
    }
    return count;
}

uint128_t getNearest4NPlus1(uint128_t n){
    while (true)
    {
        uint128_t currN = n-1;
        if (currN%4 == 0){
            break;
        }
        n = n-2; // since we start checking from an odd number, we
        // can do n=n-2 to go to the previous odd number directly
    }
    return n;
}

int getValueOfK(uint128_t n){
    return n%sieve_length;
}

int binarySearch (const int arr[], int num, int tgt){  
    int beg = 0, end = num - 1;  
    // use loop to check all sorted elements  
    while (beg <= end)  
    {  
        // get the mid value of sorted array and then compares 
        // with target element 
        int mid = (beg + end) /2;         
        if (tgt == arr[mid])  
        {  
            return mid; // when mid is equal to tgt value  
        }         
        // check tgt is less than mid value, discard left element  
        else if (tgt < arr[mid])  
        {  
            end = mid - 1;  
        }         
        // if the target is greater than the mid value, 
        // discard all elements  
        else {  
            beg = mid + 1;  
        }  
    }  
    // return -1 when target is not exists in the array  
    return -1;  
}  

void checkNNumbers(uint128_t num, uint64_t totalToCheck){
    // change num to the next odd number after the nearest 
    // odd number m where m = 4N+1 and m<n. we will be skipping 
    // numbers that are in the form of 4N+1.
    // so we start checking from num = m+2 (the odd number after m).
    num = getNearest4NPlus1(num) + 2;
    // the last number that has been checked/known to be following the 
    // conjecture (chronoloically checking from 0)
    // setting it to the number before num (even number)
    uint128_t lastCheked = num - 1;

    // used to eliminate multiple trailing zeros at once
    // int trailingZeroCount = 0;

    // number of total iterations
    const uint64_t innerIterations = totalToCheck/alpha;

    // iterator
    int i = 0; // outer loop iterator. iterates for alpha
    uint64_t j = 0; // inner loop iterator. iterates for innerIterations
    // we will make k 8 bits only. so that whenever k reaches 256, it 
    // reverts back to 0.
    uint16_t k = getValueOfK(num); // iterator to eliminate integers that converges.
    // change it to uint8_t or uint32_k based on sieve_k's value.
    
    // steps counter
    uint64_t steps = 0;

    while (i < alpha){
        // std::cout << "i: " << i << "\n" << std::endl;
        j = 0; // starting j from 0 again
        // inner loop, checking innerIteration number of integers
        while (j < innerIterations){ 
            // std::cout << "checking: " << std::endl;
            // print_int128(num);
            // std::cout << "\n" << std::endl;

            // operations
            // steps = 0; // make steps=0 here to log steps count for each integer
            // skip the checking if sieve[k] is true, it will converge
            if (!sieve[k]) {
                while(num > lastCheked){
                    if (num & 1){ // not divisible by 2
                        // currNum = ((currNum*3)+1)/2;
                        num = (num + (num << 1) + 1) >> 1;
                        // two steps (3n + 1 and dividing by 2) are performed here.
                        steps += 2;            
                    }
                    else{
                        //trailingZeroCount = count_trailing_zeros(currNum);
                        //currNum >>= trailingZeroCount;
                        num >>= 1; // right shift. dividing by 2
                        steps += 1;
                    } 
                }
            }
            j = j+4; // increment j by 4 (cause we skip even and 4n+1 numbers)
            k = k+4; // increment k by 4 because of the same reason
            // since we are checking chronologically, updating lastChecked by 4 will
            // set it's value to 3 + the number we just checked (because we started 
            // with lastChecked being num - 1) which is 1 lesser than the next 
            // number we will check (even number).
            lastCheked += 4;
            // the value of num has been changed as we were working with the
            // num variable itself, instead of using a temporary variable
            // for the loop that checks the number. so we update it to the next
            // odd integer after the just updated lastChecked
            num = lastCheked + 1;
            // std::cout << "steps checking one number: " << steps << "\n" << std::endl;
        }
        // increase i by 1
        i += 1;
    }
    std::cout << "value of j: " << std::endl;
    print_int128(j);
    std::cout << "\n" << std::endl;
    std::cout << "steps checking one core: " << std::endl;
    print_int128(steps);
    std::cout << "\n" << std::endl;
}

int main() {
    // Get the number of available cores
    const int num_cores = std::thread::hardware_concurrency();
    // Create an array of threads
    std::thread threads[num_cores];

    high_resolution_clock::time_point start, end;
    duration<double, std::micro> cpu_time_used;

    // the num to start checking from on i-th core
    uint128_t ithCoreNum;
    // number of integers we will check in one core
    const uint64_t checkInOneCore = getPower(10, MAX_POW_TEN);
    // total number of integers that will be checked
    const uint64_t totalToCheck = checkInOneCore * num_cores;
    std::cout << "Total number of integers to be checked: ";
    print_int128(totalToCheck);
    std::cout << "\n";

    // the number to start checking from
    uint128_t num = pow2(START_POW_TWO);
    // print the given value of num to start our checking from
    std::cout << "Given integer to start checking from: ";
    print_int128(num);
    std::cout << "\n";

    // make num an odd number. we will be skipping even numbers
    // so it's better to start with num being odd
    if ((num & 1) == 0){
        num = num - 1;
    }
    
    // print the actual value of num we are starting our checking from
    std::cout << "Starting checking from: ";
    print_int128(num);
    std::cout << "\n";

    // populating the sieve bitset with values of k we will skip checking for
    // first we populate with sieve values we calculated manually from the 
    // skip array (this step can be skipped)
    /*for (int i=0; i<sieve_length; i++){
        if(binarySearch(skip, skipLength, i) != -1){ sieve[i] = true; } 
    }*/

    // populate sieve with values from bitset.txt that is generated
    // by a python script.
    FILE* bitsetFile = fopen("bitset.txt", "r");
    if (bitsetFile == NULL) {
        std::cout << "Error opening file";
        return 1;
    }
    std::cout << "Bitset file opened successfully" << std::endl;
    char* bitset_str = new char[(1ULL << sieve_k)+1];
    fscanf(bitsetFile, "%s", bitset_str);
    fclose(bitsetFile);

    // Convert the bitset string array to a C++ bool array
    for (int i = 0; i < sieve_length; i++) {
        sieve[i] = (bitset_str[i] == '1') ? true : false;
    }
    std::cout << "Sieve bitset created successfully" << std::endl;

    // Record the start time
    start = high_resolution_clock::now();

    // Start the threads
    for (int i = 0; i < num_cores; ++i) {
        ithCoreNum = num + (i*checkInOneCore);
        threads[i] = std::thread(checkNNumbers, ithCoreNum, checkInOneCore);
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_cores; ++i) {
        threads[i].join();
    }

    // Record the end time
    end = high_resolution_clock::now();

    // calculate the CPU time used 
    cpu_time_used = duration_cast<duration<double, std::micro>>(end - start);
    std::cout << "Time taken: " << cpu_time_used.count() / 1000000 << " seconds" << std::endl;

    // cleanup
    delete[] sieve;
    delete[] bitset_str;

    return 0;
}