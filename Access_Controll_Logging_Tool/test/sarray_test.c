#include "../lib/Misc.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

const char *lines[] = {
    "And disciplinary remains mercifully",
    "Yes and um, I'm with you Derek, this star nonsense",
    "Yes, yes",
    "Now which is it?",
    "I am sure of it",
    "So, so you think you can tell",
    "Heaven from hell?",
    "Blue skies from pain?",
    "Can you tell a green field",
    "From a cold steel rail?",
    "A smile from a veil?",
    "Do you think you can tell?",
    "Did they get you to trade",
    "Your heroes for ghosts?",
    "Hot ashes for trees?",
    "Hot air for a cool breeze?",
    "Cold comfort for change?",
    "Did you exchange",
    "A walk-on part in the war",
    "For a lead role in a cage?",
    "How I wish, how I wish you were here",
    "We're just two lost souls",
    "Swimming in a fish bowl",
    "Year after year",
    "Running over the same old ground",
    "What have we found?",
    "The same old fears",
    "Wish you were here"
};

#define assert(cond, message) if (!(cond)) { printf(RED "FAILED" RESET); printf("\t %s\n", message); return 0; }

String_array_t UUT;

/**
 * @test Test the append function of @link Misk.h @endlink.
*/
int single_append_test_(){
    for(int i = 0; i < 28; i++){
        PushStringArray(&UUT, lines[i]);

        assert(strcmp(readStringArray(&UUT, i), lines[i]) == 0, "String is not correct");
        assert(UUT.size == (unsigned int)(i + 1), "Size is not correct");
    }
    return 1;
}

/**
 * @test Test the append function of @link Misk.h @endlink.
*/
int whole_append_test(){
    for(int i = 0; i < 28; i++){
        PushStringArray(&UUT, lines[i]);
    }

    assert(UUT.size == (unsigned int)28, "Size is not correct");

    for(int i = 0; i < 28; i++){
        assert(strcmp(readStringArray(&UUT, i), lines[i]) == 0, "String is not correct");
    }
    return 1;
}   

/**
 * @test Test the set function of @link Misk.h @endlink.
*/
int single_set_test(){
    for(int i = 0; i < 28; i++)
        PushStringArray(&UUT, lines[i]);
    

    for(int i = 0; i < 28; i++){
        setStringArray(&UUT, i, lines[27 - i]);

        assert(strcmp(readStringArray(&UUT, i), lines[27 - i]) == 0, "String is not correct");
        assert(UUT.size == (unsigned int)28, "Size is not correct");
    }
    return 1;
}

/**
 * @test Test the set function of @link Misk.h @endlink.
*/
int whole_set_test(){
    for(int i = 0; i < 28; i++)
        PushStringArray(&UUT, lines[i]);

    for(int i = 0; i < 28; i++){
        setStringArray(&UUT, i, lines[27 - i]);
        assert(UUT.size == (unsigned int)28, "Size is not correct");
    }

    for(int i = 0; i < 28; i++)
        assert(strcmp(readStringArray(&UUT, i), lines[27 - i]) == 0, "String is not correct");
    
    return 1;
}

/**
 * @test Test the free function of @link Misk.h @endlink.
*/
int free_test(){
    for(int i = 0; i < 28; i++)
        PushStringArray(&UUT, lines[i]);


    FreeStringArray(&UUT);

    assert(UUT.data == NULL, "Data is not NULL");
    assert(UUT.size == 0, "Size is not 0");
    assert(UUT.capacity == 0, "Capacity is not 0");

    return 1;
}

int main(){
    UUT = InitStringArray(50);

    printf("%-20s : ", "Free");
    printf("[%4s]\n", free_test() ? GRN "PASS" RESET : RED "FAIL" RESET);

    UUT = InitStringArray(50);

    printf("%-20s : ", "Single Append");
    printf("[%4s]\n", single_append_test_() ? GRN "PASS" RESET : RED "FAIL" RESET);
    
    FreeStringArray(&UUT);
    UUT = InitStringArray(50);

    printf("%-20s : ", "Whole Append");
    printf("[%4s]\n", whole_append_test() ? GRN "PASS" RESET : RED "FAIL" RESET);
    
    FreeStringArray(&UUT);
    UUT = InitStringArray(50);

    printf("%-20s : ", "Single Set");
    printf("[%4s]\n", single_set_test() ? GRN "PASS" RESET : RED "FAIL" RESET);

    FreeStringArray(&UUT);
    UUT = InitStringArray(50);

    printf("%-20s : ", "Whole Set");
    printf("[%4s]\n", whole_set_test() ? GRN "PASS" RESET : RED "FAIL" RESET);

    FreeStringArray(&UUT);

    return 0;
}