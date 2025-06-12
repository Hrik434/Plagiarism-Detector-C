#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_LINE_LENGTH 1024
#define MAX_WORD_LENGTH 100
#define INITIAL_BUFFER_SIZE 50
#define BUFFER_INCREMENT 50
#define MAX_SYNONYM_LENGTH 500
#define HASH_MAP_SIZE 50021
#define IDF_for_uncommon_words 3
#define IDF_for_common 2

/////////////////////////////////////////////////////////////////////////
//maximum number between two integers
int max(int a,int b){
    if(a>=b){
        return a;
    }
    else{
        return b;
    }
}

////////////////////////////////////////////////////////////
//reading from a .txt,.csv,.json file

char* read_file(const char* filename) {

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Error opening file");
        return NULL;
    }

    int size_of_string = MAX_LINE_LENGTH;
    char *the_string = malloc(size_of_string);
    if (!the_string) {
        perror("Memory allocation failed");
        fclose(f);
        return NULL;
    }

    int string_length = 0;

    char temporary_string[512];
    while (fgets(temporary_string, sizeof(temporary_string), f)) {
        size_t temporary_string_length = strlen(temporary_string);

        if (string_length + temporary_string_length + 1 > size_of_string) {
            size_of_string *= 2;
            char *temp = realloc(the_string, size_of_string);
            if (!temp) {
                perror("Realloc failed");
                free(the_string);
                fclose(f);
                return NULL;
            }
            the_string = temp;
        }

        for (int i = 0; i < temporary_string_length; i++) {
            if (temporary_string[i] == '\n') {
                temporary_string[i] = ' ';
            }
        }

        memcpy(the_string + string_length, temporary_string, temporary_string_length);
        string_length += temporary_string_length;
    }

    the_string[string_length] = '\0';

    fclose(f);
    return the_string;
}

/////////////////////////////////////////////////////////////////
//deleting all the punctuation marks and special characters , everything except alphabets and numbers

char* clean_string(const char *in) {
    int string_length = strlen(in);
    char *out = malloc(string_length + 1);
    if (!out) {
        perror("Memory allocation failed");
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < string_length; i++) {

        if (isalnum(in[i]) || in[i] == ' ') {

            if (j > 0 && in[i] == ' ' && out[j - 1] == ' ') {
                continue;
            }
            out[j] = in[i];
            j++;
        }
    }

    out[j] = '\0';
    return out;
}

/////////////////////////////////////////////////////////////
//turning all the uppercase letters into lowercase

char* convert_to_lowercase(const char *in) {
    int string_length = strlen(in);
    char *out = malloc(string_length + 1);
    if (!out) {
        perror("Memory allocation failed");
        return NULL;
    }

    for (int i = 0; i < string_length; i++) {

        out[i] = tolower(in[i]);
    }
    out[string_length]='\0';

    return out;
}


//////////////////////////////////////////////////////////////////////////////
//count words in the string

int count_words_in_string(const char *input) {
    int count = 0;
    int inside_word = 0;

    for (size_t i = 0; i< strlen(input); i++) {
        if (isalnum(input[i])) {
            if (!inside_word) {
                count++;
                inside_word = 1;
            }
        } else {
            inside_word = 0;
        }
    }
    return count;
}

/////////////////////////////////////////////////////////////////////////
//getting a string array

char** string_to_array(const char *input, int n) {
    char **words =  calloc(n + 1, sizeof(char*));
    if (!words) {
        perror("Memory allocation failed");
        return NULL;
    }

    int word_count = 0;
    char word[100];
    int word_length = 0;

    for (size_t i = 0; i<strlen(input); i++) {
        if (isalnum(input[i])) {
            word[word_length++] = input[i];
        }
        else if (word_length > 0) {
            word[word_length] = '\0';
            words[word_count] = strdup(word);
            word_count++;
            word_length = 0;
        }
    }

    if (word_length > 0) {
        word[word_length] = '\0';
        words[word_count] = strdup(word);
    }

    return words;
}

//////////////////////////////////////////////////////////////////
//stopword remover

int is_stopword(const char *word) {

    const char *stopwords[] = {
        "i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "your",
        "yours", "yourself", "yourselves", "he", "him", "his", "himself", "she",
        "her", "hers", "herself", "it", "its", "itself", "they", "them", "their",
        "theirs", "themselves", "what", "which", "who", "whom", "this", "that",
        "these", "those", "am", "is", "are", "was", "were", "be", "been", "being",
        "have", "has", "had", "having", "do", "does", "did", "doing", "a", "an", "the",
        "and", "but", "if", "or", "because", "as", "until", "while", "of", "at", "by",
        "for", "with", "about", "against", "between", "into", "through", "during",
        "before", "after", "above", "below", "to", "from", "up", "down", "in", "out",
        "on", "off", "over", "under", "again", "further", "then", "once", "here",
        "there", "when", "where", "why", "how", "all", "any", "both", "each", "few",
        "more", "most", "other", "some", "such", "no", "nor", "not", "only", "own",
        "same", "so", "than", "too", "very", "s", "t", "can", "will", "just", "don",
        "should", "now", "d", "ll", "m", "o", "re", "ve", "y", "ain't", "aren't", "couldn't",
        "didn't", "doesn't", "hadn't", "hasn't", "haven't", "isn't", "ma", "mightn't",
        "mustn't", "needn't", "shan't", "shouldn't", "wasn't", "weren't", "won't", "wouldn't"
    };
    int number_of_stopwords = 153;

    for (size_t i = 0; i < number_of_stopwords; i++) {
        if (strcmp(word, stopwords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

char* remove_stopwords_string(const char *input) {
    size_t len = strlen(input);
    size_t cap = len + 1;
    char *output = malloc(cap);
    if (!output) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    size_t j = 0;
    char word[100];
    size_t word_len = 0;

    for (size_t i = 0; i <= len; i++) {
        if (isalnum(input[i])) {
            word[word_len++] = input[i];
        } else if (word_len > 0) {
            word[word_len] = '\0';

            if (!is_stopword(word)) {
                for (size_t k = 0; k < word_len; k++) {
                    output[j++] = word[k];
                }
                output[j++] = ' ';
            }
            word_len = 0;
        }
    }
    output[j] = '\0';
    return output;
}

///////////////////////////////////////////////////////////////////////////
//counting strings in the array

int count_words_in_array(char **words) {
    int count = 0;
    while (words[count] != NULL) {
        count++;
    }
    return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
//synonym replace

typedef struct SynonymNode {
    char word[MAX_WORD_LENGTH];
    char canonical_synonym[MAX_WORD_LENGTH];
    struct SynonymNode* next;
} SynonymNode;

static SynonymNode* synonym_hash_map[HASH_MAP_SIZE] = { NULL };  // Global hash map

// djb2 hash function
unsigned int hash_function(const char *word) {
    unsigned long hash = 5381;
    int c;
    while ((c = *word++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return (unsigned int)(hash % HASH_MAP_SIZE);
}

// Insert a word and its canonical synonym into the hash map
void insert_synonym(const char* word, const char* canonical) {
    unsigned int hash_index = hash_function(word);

    SynonymNode* new_node = malloc(sizeof(SynonymNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(new_node->word, word, MAX_WORD_LENGTH);
    strncpy(new_node->canonical_synonym, canonical, MAX_WORD_LENGTH);
    new_node->next = synonym_hash_map[hash_index];
    synonym_hash_map[hash_index] = new_node;
}

// Load synonyms from file into hash map (call once)
void load_synonyms(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[MAX_SYNONYM_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char word[MAX_WORD_LENGTH];
        char synonyms[MAX_SYNONYM_LENGTH];

        if (sscanf(line, "%99[^:]:%499[^\n]", word, synonyms) != 2) {
            // Skip malformed lines
            continue;
        }

        char *first_synonym = strtok(synonyms, ",");
        if (!first_synonym) {
            // If no synonym found, use the word itself as canonical synonym
            first_synonym = word;
        }

        insert_synonym(word, first_synonym);
    }
    fclose(file);
}

// Lookup function: input a word, output its canonical synonym
const char* get_common_synonym(const char* word) {
    unsigned int hash_index = hash_function(word);
    SynonymNode* current = synonym_hash_map[hash_index];

    while (current) {
        if (strcmp(current->word, word) == 0) {
            return current->canonical_synonym;
        }
        current = current->next;
    }

    return word;  // Return the original word if not found
}

// Optional: free all allocated memory before program ends
void free_hash_map() {
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        SynonymNode* current = synonym_hash_map[i];
        while (current) {
            SynonymNode* temp = current;
            current = current->next;
            free(temp);
        }
        synonym_hash_map[i] = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////////
//stemmer

struct stemmer {
    char *b;
    int k;
    int k0;
    int j;
    int capacity;
};

// Function prototypes
struct stemmer *create_stemmer(void);
void free_stemmer(struct stemmer *z);
int stem(struct stemmer *z, const char *word, int len);
static int cons(struct stemmer *z, int i);
static int m(struct stemmer *z);
static int vowelinstem(struct stemmer *z);
static int doublec(struct stemmer *z, int j);
static int cvc(struct stemmer *z, int i);
static int ends(struct stemmer *z, const char *s, int len);
static void setto(struct stemmer *z, const char *s, int len);
static void r(struct stemmer *z, const char *s, int len);
static void step1ab(struct stemmer *z);
static void step1c(struct stemmer *z);
static void step2(struct stemmer *z);
static void step3(struct stemmer *z);
static void step4(struct stemmer *z);
static void step5(struct stemmer *z);
static int resize_buffer(struct stemmer *z, int new_size);

// Create a new stemmer
struct stemmer *create_stemmer(void) {
    struct stemmer *z = (struct stemmer *)malloc(sizeof(struct stemmer));
    if (z == NULL) return NULL;
    z->b = (char *)malloc(INITIAL_BUFFER_SIZE);
    if (z->b == NULL) {
        free(z);
        return NULL;
    }
    z->capacity = INITIAL_BUFFER_SIZE;
    z->k = 0;
    z->k0 = 0;
    z->j = 0;
    return z;
}

// Free stemmer resources
void free_stemmer(struct stemmer *z) {
    if (z) {
        free(z->b);
        free(z);
    }
}

// Resize the stemmer's buffer
static int resize_buffer(struct stemmer *z, int new_size) {
    if (new_size <= z->capacity) return 1;
    int new_capacity = z->capacity;
    while (new_capacity < new_size) new_capacity += BUFFER_INCREMENT;
    char *new_b = (char *)realloc(z->b, new_capacity);
    if (new_b == NULL) return 0;
    z->b = new_b;
    z->capacity = new_capacity;
    return 1;
}

// Check if character at position i is a consonant
static int cons(struct stemmer *z, int i) {
    switch (z->b[i]) {
        case 'a': case 'e': case 'i': case 'o': case 'u': return 0;
        case 'y': return (i == z->k0) ? 1 : !cons(z, i - 1);
        default: return 1;
    }
}

// Measure the number of consonant sequences (m)
static int m(struct stemmer *z) {
    int n = 0;
    int i = z->k0;
    while (1) {
        if (i > z->j) return n;
        if (!cons(z, i)) break;
        i++;
    }
    i++;
    while (1) {
        while (1) {
            if (i > z->j) return n;
            if (cons(z, i)) break;
            i++;
        }
        i++;
        n++;
        while (1) {
            if (i > z->j) return n;
            if (!cons(z, i)) break;
            i++;
        }
        i++;
    }
}

// Check if there is a vowel in the stem
static int vowelinstem(struct stemmer *z) {
    for (int i = z->k0; i <= z->j; i++)
        if (!cons(z, i)) return 1;
    return 0;
}

// Check for double consonant at position j
static int doublec(struct stemmer *z, int j) {
    if (j < z->k0 + 1) return 0;
    if (z->b[j] != z->b[j - 1]) return 0;
    return cons(z, j);
}

// Check for CVC pattern at position i
static int cvc(struct stemmer *z, int i) {
    if (i < z->k0 + 2 || !cons(z, i) || cons(z, i - 1) || !cons(z, i - 2)) return 0;
    int ch = z->b[i];
    if (ch == 'w' || ch == 'x' || ch == 'y') return 0;
    return 1;
}

// Check if word ends with string s of length len
static int ends(struct stemmer *z, const char *s, int len) {
    if (len > z->k - z->k0 + 1) return 0;
    if (z->b[z->k - len + 1] != s[0]) return 0; // Tiny speed-up
    if (strncmp(z->b + z->k - len + 1, s, len) != 0) return 0;
    z->j = z->k - len;
    return 1;
}

//Set the ending to string s of length len
static void setto(struct stemmer *z, const char *s, int len) {
    if (!resize_buffer(z, z->j + len + 1)) return;
    memmove(z->b + z->j + 1, s, len);
    z->k = z->j + len;
}

// Replace ending if m > 0
static void r(struct stemmer *z, const char *s, int len) {
    if (m(z) > 0) setto(z, s, len);
}

// Step 1ab: Handle -s and related suffixes
static void step1ab(struct stemmer *z) {
    if (z->b[z->k] == 's') {
        if (ends(z, "sses", 4)) z->k -= 2;
        else if (ends(z, "ies", 3)) setto(z, "i", 1);
        else if (z->b[z->k - 1] != 's') z->k--;
    }
    if (ends(z, "eed", 3)) {
        if (m(z) > 0) z->k--;
    } else if ((ends(z, "ed", 2) || ends(z, "ing", 3)) && vowelinstem(z)) {
        z->k = z->j;
        if (ends(z, "at", 2)) setto(z, "ate", 3);
        else if (ends(z, "bl", 2)) setto(z, "ble", 3);
        else if (ends(z, "iz", 2)) setto(z, "ize", 3);
        else if (doublec(z, z->k)) {
            z->k--;
            int ch = z->b[z->k];
            if (ch == 'l' || ch == 's' || ch == 'z') z->k++;
        } else if (m(z) == 1 && cvc(z, z->k)) setto(z, "e", 1);
    }
}

// Step 1c: Handle -y to -i
static void step1c(struct stemmer *z) {
    if (ends(z, "y", 1) && vowelinstem(z)) setto(z, "i", 1);
}

// Step 2: Handle suffixes like -ational, -tional, etc.
static void step2(struct stemmer *z) {
    switch (z->b[z->k - 1]) {
        case 'a':
            if (ends(z, "ational", 7)) { r(z, "ate", 3); break; }
            if (ends(z, "tional", 6)) { r(z, "tion", 4); break; }
            break;
        case 'c':
            if (ends(z, "enci", 4)) { r(z, "ence", 4); break; }
            if (ends(z, "anci", 4)) { r(z, "ance", 4); break; }
            break;
        case 'e':
            if (ends(z, "izer", 4)) { r(z, "ize", 3); break; }
            break;
        case 'l':
            if (ends(z, "abli", 4)) { r(z, "able", 4); break; }
            if (ends(z, "alli", 4)) { r(z, "al", 2); break; }
            if (ends(z, "entli", 5)) { r(z, "ent", 3); break; }
            if (ends(z, "eli", 3)) { r(z, "e", 1); break; }
            if (ends(z, "ousli", 5)) { r(z, "ous", 3); break; }
            break;
        case 'o':
            if (ends(z, "ization", 7)) { r(z, "ize", 3); break; }
            if (ends(z, "ation", 5)) { r(z, "ate", 3); break; }
            if (ends(z, "ator", 4)) { r(z, "ate", 3); break; }
            break;
        case 's':
            if (ends(z, "alism", 5)) { r(z, "al", 2); break; }
            if (ends(z, "iveness", 7)) { r(z, "ive", 3); break; }
            if (ends(z, "fulness", 7)) { r(z, "ful", 3); break; }
            if (ends(z, "ousness", 7)) { r(z, "ous", 3); break; }
            break;
        case 't':
            if (ends(z, "aliti", 5)) { r(z, "al", 2); break; }
            if (ends(z, "iviti", 5)) { r(z, "ive", 3); break; }
            if (ends(z, "biliti", 6)) { r(z, "ble", 3); break; }
            break;
        case 'g':
            if (ends(z, "logi", 4)) { r(z, "log", 3); break; }
            break;
    }
}

// Step 3: Handle suffixes like -icate, -ative, etc.
static void step3(struct stemmer *z) {
    switch (z->b[z->k]) {
        case 'e':
            if (ends(z, "icate", 5)) { r(z, "ic", 2); break; }
            if (ends(z, "ative", 5)) { r(z, "", 0); break; }
            if (ends(z, "alize", 5)) { r(z, "al", 2); break; }
            break;
        case 'i':
            if (ends(z, "iciti", 5)) { r(z, "ic", 2); break; }
            break;
        case 'l':
            if (ends(z, "ical", 4)) { r(z, "ic", 2); break; }
            if (ends(z, "ful", 3)) { r(z, "", 0); break; }
            break;
        case 's':
            if (ends(z, "ness", 4)) { r(z, "", 0); break; }
            break;
    }
}

//Step 4: Handle suffixes like -ant, -ence, etc.
static void step4(struct stemmer *z) {
    switch (z->b[z->k - 1]) {
        case 'a':
            if (ends(z, "al", 2)) break;
            return;
        case 'c':
            if (ends(z, "ance", 4)) break;
            if (ends(z, "ence", 4)) break;
            return;
        case 'e':
            if (ends(z, "er", 2)) break;
            return;
        case 'i':
            if (ends(z, "ic", 2)) break;
            return;
        case 'l':
            if (ends(z, "able", 4)) break;
            if (ends(z, "ible", 4)) break;
            return;
        case 'n':
            if (ends(z, "ant", 3)) break;
            if (ends(z, "ement", 5)) break;
            if (ends(z, "ment", 4)) break;
            if (ends(z, "ent", 3)) break;
            return;
        case 'o':
            if (ends(z, "ion", 3) && z->j >= z->k0 && (z->b[z->j] == 's' || z->b[z->j] == 't')) break;
            return;
        case 's':
            if (ends(z, "ism", 3)) break;
            return;
        case 't':
            if (ends(z, "ate", 3)) break;
            if (ends(z, "iti", 3)) break;
            return;
        case 'u':
            if (ends(z, "ous", 3)) break;
            return;
        case 'v':
            if (ends(z, "ive", 3)) break;
            return;
        case 'z':
            if (ends(z, "ize", 3)) break;
            return;
        default:
            return;
    }
    if (m(z) > 1) z->k = z->j;
}

// Step 5: Handle final -e and -l
static void step5(struct stemmer *z) {
    z->j = z->k;
    if (z->b[z->k] == 'e') {
        int a = m(z);
        if (a > 1 || (a == 1 && !cvc(z, z->k - 1))) z->k--;
    }
    if (z->b[z->k] == 'l' && doublec(z, z->k) && m(z) > 1) z->k--;
}

//Main stemming function
int stem(struct stemmer *z, const char *word, int len) {
    if (!z || !word || len < 0) return -1;
    if (len <= 1) return len; // No stemming for short words
    if (!resize_buffer(z, len + 1)) return -1;

    for (int i = 0; i < len; i++) {
        if (!isalpha((unsigned char)word[i])) return -1; // Only alphabetic characters
        z->b[i] = tolower((unsigned char)word[i]);
    }
    z->b[len] = '\0';
    z->k = len - 1;
    z->k0 = 0;
    z->j = 0;

    step1ab(z);
    if (z->k > z->k0) {
        step1c(z);
        step2(z);
        step3(z);
        step4(z);
        step5(z);
    }
    return z->k + 1;
}

char *stem_word(const char *word) {
    if (!word) return word;
    int len = strlen(word);
    if (len <= 1) {
        char *result = (char *)malloc(len + 1);
        if (!result) return word;
        strcpy(result, word);
        return result;
    }

    struct stemmer *z = create_stemmer();
    if (!z) return word;

    int new_len = stem(z, word, len);
    if (new_len < 0) {
        free_stemmer(z);
        return word;
    }

    char *result = (char *)malloc(new_len + 1);
    if (!result) {
        free_stemmer(z);
        return word;
    }

    memcpy(result, z->b, new_len);
    result[new_len] = '\0';
    free_stemmer(z);
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////
//counting common words

char** find_common_strings(char **arr1, int size1, char **arr2, int size2, int *common_count) {
    char **common = malloc(sizeof(char*) * (size1 < size2 ? size1 : size2));
    int count = 0;

    for (int i = 0; i < size1; i++) {
        for (int j = 0; j < size2; j++) {
            if (strcmp(arr1[i], arr2[j]) == 0) {
                int already_added = 0;
                for (int k = 0; k < count; k++) {
                    if (strcmp(common[k], arr1[i]) == 0) {
                        already_added = 1;
                        break;
                    }
                }
                if (!already_added) {
                    common[count] = strdup(arr1[i]);
                    count++;
                }
            }
        }
    }

    *common_count = count;
    return common;
}

///////////////////////////////////////////////////////////////////////////////////////
//finding uncommon words

char** find_uncommon_strings(char **arr1, int size1, char **arr2, int size2, int *uncommon_count) {
    char **uncommon = malloc(sizeof(char*) * (size1 + size2));
    int count = 0;

    for (int i = 0; i < size1; i++) {
        int found = 0;
        for (int j = 0; j < size2; j++) {
            if (strcmp(arr1[i], arr2[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            int already_added = 0;
            for (int k = 0; k < count; k++) {
                if (strcmp(uncommon[k], arr1[i]) == 0) {
                    already_added = 1;
                    break;
                }
            }
            if (!already_added) {
                uncommon[count] = strdup(arr1[i]);
                count++;
            }
        }
    }

    for (int i = 0; i < size2; i++) {
        int found = 0;
        for (int j = 0; j < size1; j++) {
            if (strcmp(arr2[i], arr1[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            int already_added = 0;
            for (int k = 0; k < count; k++) {
                if (strcmp(uncommon[k], arr2[i]) == 0) {
                    already_added = 1;
                    break;
                }
            }
            if (!already_added) {
                uncommon[count] = strdup(arr2[i]);
                count++;
            }
        }
    }

    *uncommon_count = count;
    return uncommon;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//finding union function

int contains(char **arr, int size, const char *word) {
    for (int i = 0; i < size; i++) {
        if (strcmp(arr[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

char** union_arrays(char **arr1, int size1, char **arr2, int size2, int *union_count) {

    char **union_arr = malloc(sizeof(char*) * (size1 + size2));
    int count = 0;

    for (int i = 0; i < size1; i++) {
        if (!contains(union_arr, count, arr1[i])) {
            union_arr[count] = strdup(arr1[i]);
            count++;
        }
    }

    for (int i = 0; i < size2; i++) {
        if (!contains(union_arr, count, arr2[i])) {
            union_arr[count] = strdup(arr2[i]);
            count++;
        }
    }

    *union_count = count;
    return union_arr;
}

////////////////////////////////////////////////////////////////////////////////////////
//getting tf_idf

double* calculate_tfidf(char **doc1, int rows1, char **doc2, int rows2, char **union_arr, int rows_union, int *result_size) {

    double *tfidf = (double *)malloc(rows_union * sizeof(double));
    *result_size = rows_union;

    for (int i = 0; i < rows_union; i++) {
        int count_in_doc1 = 0;
        for (int j = 0; j < rows1; j++) {
            if (strcmp(union_arr[i], doc1[j]) == 0) {
                count_in_doc1++;
            }
        }
        double tf = (double)count_in_doc1 / rows1;
        int in_doc1 = count_in_doc1 > 0 ? 1 : 0;
        int in_doc2 = 0;
        for (int j = 0; j < rows2; j++) {
            if (strcmp(union_arr[i], doc2[j]) == 0) {
                in_doc2 = 1;
                break;
            }
        }

        double idf = (in_doc1 && !in_doc2) || (!in_doc1 && in_doc2) ? IDF_for_uncommon_words : IDF_for_common;
        tfidf[i] = tf * idf;
    }

    return tfidf;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//cosine similarity

double cosine_similarity(double *A, double *B, int size) {
    double dot = 0.0, mag_A = 0.0, mag_B = 0.0;

    for (int i = 0; i < size; i++) {
        dot += A[i] * B[i];
        mag_A += A[i] * A[i];
        mag_B += B[i] * B[i];
    }

    if (mag_A == 0.0 || mag_B == 0.0) {
        return 0.0;
    }

    return dot / (sqrt(mag_A) * sqrt(mag_B));
}

///////////////////////////////////////////////////////////////////////////////////////////
//levenshtein distance

int levenshtein_distance(const char *str1, const char *str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    int *prev_row = malloc((len2 + 1) * sizeof(int));
    int *curr_row = malloc((len2 + 1) * sizeof(int));

    for (int j = 0; j <= len2; j++) {
        prev_row[j] = j;
    }

    for (int i = 1; i <= len1; i++) {
        curr_row[0] = i;
        for (int j = 1; j <= len2; j++) {
            int cost = (str1[i - 1] == str2[j - 1]) ? 0 : 1;
            int insertion = curr_row[j - 1] + 1;
            int deletion = prev_row[j] + 1;
            int substitution = prev_row[j - 1] + cost;

            int min = insertion < deletion ? insertion : deletion;
            curr_row[j] = min < substitution ? min : substitution;
        }
        int *temp = prev_row;
        prev_row = curr_row;
        curr_row = temp;
    }

    int result = prev_row[len2];
    free(prev_row);
    free(curr_row);
    return result;
}


/////////////////////////////////////////////////////////////////////////////////////////
//jaccard similarity

double jaccard_similarity(int a, int b){
    double p =a,q=b;
    return p/q;
}

//////////////////////////////////////////////////////////////////////////////////////////
//main function

int main() {
    load_synonyms("hashmap.txt");

    const char *filename1 = "file1.txt";
    const char *filename2 = "file2.txt";

    char *content1 = read_file(filename1);
    char *content2 = read_file(filename2);

    if (!content1 || !content2) {
        printf("Error reading files.\n");
        return 1;
    }

    char *file1_a = clean_string(content1);
    char *file2_a = clean_string(content2);

    if (!file1_a || !file2_a) {
        printf("Error cleaning strings.\n");
        free(content1);
        free(content2);
        return 1;
    }

    char *file1_b = convert_to_lowercase(file1_a);
    char *file2_b = convert_to_lowercase(file2_a);

    if (!file1_b || !file2_b) {
        printf("Error converting to lowercase.\n");
        free(file1_a);
        free(file2_a);
        free(content1);
        free(content2);
        return 1;
    }

    char *file1_c = remove_stopwords_string(file1_b);
    char *file2_c = remove_stopwords_string(file2_b);

    if (!file1_c || !file2_c) {
        printf("Error removing stopwords.\n");
        free(file1_b);
        free(file2_b);
        free(file1_a);
        free(file2_a);
        free(content1);
        free(content2);
        return 1;
    }

    int file1_c_count = count_words_in_string(file1_c);
    int file2_c_count = count_words_in_string(file2_c);

    char **file1_d = string_to_array(file1_c, file1_c_count);
    char **file2_d = string_to_array(file2_c, file2_c_count);

    if (!file1_d || !file2_d) {
        printf("Error converting strings to arrays.\n");
        free(file1_c);
        free(file2_c);
        free(file1_b);
        free(file2_b);
        free(file1_a);
        free(file2_a);
        free(content1);
        free(content2);
        return 1;
    }

    char file1_e[file1_c_count][MAX_WORD_LENGTH];
    char file2_e[file2_c_count][MAX_WORD_LENGTH];

    for (int i = 0; i < file1_c_count; i++) {
        strcpy(file1_e[i], get_common_synonym(file1_d[i]));
    }

    for (int i = 0; i < file2_c_count; i++) {
        strcpy(file2_e[i], get_common_synonym(file2_d[i]));
    }

    char **file1_f = malloc(file1_c_count * sizeof(char *));
    char **file2_f = malloc(file2_c_count * sizeof(char *));
    if (!file1_f || !file2_f) {
        printf("Memory allocation failed for stemmed words.\n");
        free(file1_d);
        free(file2_d);
        free(file1_c);
        free(file2_c);
        free(file1_b);
        free(file2_b);
        free(file1_a);
        free(file2_a);
        free(content1);
        free(content2);
        return 1;
    }

    for (int i = 0; i < file1_c_count; i++) {
        file1_f[i] = stem_word(file1_e[i]);
    }

    for (int i = 0; i < file2_c_count; i++) {
        file2_f[i] = stem_word(file2_e[i]);
    }

    int common_count = 0;
    char **common = find_common_strings(file1_f, file1_c_count, file2_f, file2_c_count, &common_count);

    int uncommon_count = 0;
    char **uncommon = find_uncommon_strings(file1_f, file1_c_count, file2_f, file2_c_count, &uncommon_count);

    int union_count = 0;
    char **union_array = union_arrays(file1_f, file1_c_count, file2_f, file2_c_count, &union_count);

    int tf_idf_count = 0;
    double *tf_idf_file1 = calculate_tfidf(file1_f, file1_c_count, file2_f, file2_c_count, union_array, union_count, &tf_idf_count);
    double *tf_idf_file2 = calculate_tfidf(file2_f, file2_c_count, file1_f, file1_c_count, union_array, union_count, &tf_idf_count);

    double cosine = cosine_similarity(tf_idf_file1, tf_idf_file2, union_count);

    int distance = levenshtein_distance(file1_c, file2_c);

    double levenshtein = 1 - (distance / (double)max(strlen(file1_c), strlen(file2_c)));

    double jaccard = jaccard_similarity(common_count, union_count);

    double final_percentage = 100 * (cosine * 0.30 + levenshtein * 0.45 + jaccard * 0.25);

    int uncommon_count_show = 0;
    char **uncommon_show = find_uncommon_strings(file1_d, file1_c_count, file2_d, file2_c_count, &uncommon_count_show);
    int common_count_show = 0;
    char **common_show = find_common_strings(file1_d, file1_c_count, file2_d, file2_c_count, &common_count_show);

    printf("Unique words in file 1 : \n\n");

    int j, num1 = 0, num2 = 0, num3 = 0;

    for(int i=0;i<uncommon_count_show;i++){
        for(j=0;j<file1_c_count;j++){
            if(strcmp(uncommon_show[i],file1_d[j])==0){
                break;
            }
        }
        if(j<file1_c_count){
            printf("%s\n",uncommon_show[i]);
            num1++;
        }
    }

    printf("\n\n");

    printf("Total unique words in file 1 : %d\n\n",num1);

    printf("Unique words in file 2 : \n\n");

    for(int i=0;i<uncommon_count_show;i++){
        for(j=0;j<file2_c_count;j++){
            if(strcmp(uncommon_show[i],file2_d[j])==0){
                break;
            }
        }
        if(j<file2_c_count){
            printf("%s\n",uncommon_show[i]);
            num2++;
        }
    }

    printf("\n\n");

    printf("Total unique words in file 2 : %d\n\n",num2);

    printf("Common  words between the two given files : \n\n");

    for(int i=0;i<common_count_show;i++){
        printf("%s\n",common_show[i]);
        num3++;
    }

    printf("\n\n");

    printf("Total common words between the two files : %d\n\n",num3);

    printf("Plagiarism : %0.2lf%%\n", final_percentage);

    if(final_percentage>50.00){
        printf("Possible Plagiarism Detected.\n");
    }
    else{
        printf("Documents appear sufficiently different.\n");
    }

    // Free all allocated memory
    free(content1);
    free(content2);
    free(file1_a);
    free(file2_a);
    free(file1_b);
    free(file2_b);
    free(file1_c);
    free(file2_c);
    free(file1_d);
    free(file2_d);
    for (int i = 0; i < file1_c_count; i++) {
        free(file1_f[i]);
    }
    for (int i = 0; i < file2_c_count; i++) {
        free(file2_f[i]);
    }
    free(file1_f);
    free(file2_f);
    free(common);
    free(uncommon);
    free(union_array);
    free(tf_idf_file1);
    free(tf_idf_file2);
    free_hash_map();

    return 0;
}
