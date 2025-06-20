# 📚 Document Plagiarism Checker (C Language)

## 🌟 Overview

This repository hosts a robust document plagiarism checker implemented in C. It is designed to analyze two input text files, preprocess their content, and then calculate a plagiarism score based on several well-known similarity metrics. The tool provides insights into unique and common words between the documents, ultimately determining the likelihood of plagiarism. This project is ideal for students, educators, and anyone needing a fundamental text comparison utility.

## ✨ Features

* **File Reading:** Handles `.txt`, `.csv`, and `.json` files efficiently.

* **Text Preprocessing:**

    * Removes punctuation and special characters.

    * Converts text to lowercase for case-insensitive comparison.

    * Eliminates common English stopwords.

    * Replaces synonyms with canonical forms using a hash map (requires `hashmap.txt`).

    * Splits text into individual words.

    * Applies Porter Stemming to reduce words to their root form.

* **Similarity Metrics:**

    * **Cosine Similarity:** Compares document vectors based on TF-IDF.

    * **Levenshtein Distance:** Measures the edit distance between texts.

    * **Jaccard Similarity:** Quantifies similarity based on the ratio of common to total unique words.

* **Comprehensive Output:** Displays unique words per file, common words, and a final plagiarism percentage.

* **Plagiarism Detection:** Flags potential plagiarism if the calculated percentage exceeds a configurable threshold (currently 50%).

* **Memory Management:** Proper allocation and deallocation of memory to prevent leaks.

## 🚀 Getting Started

### Prerequisites

To compile and run this program, you will need:

* A C compiler (e.g., GCC).

* A `hashmap.txt` file in the same directory as the executable for synonym replacement. An example format for `hashmap.txt` is:

    ```
    word1:synonym1,synonym2,synonym3
    word2:synonym_a,synonym_b
    ```

    The first synonym listed will be considered the canonical form.

* Two input text files (e.g., `file1.txt` and `file2.txt`) for comparison, also in the same directory as the executable.

### Installation and Compilation

1.  **Clone the repository:**

    ```bash
    git clone [https://github.com/Hrik434/Plagiarism-Detector-C.git](https://github.com/Hrik434/Plagiarism-Detector-C.git)
    cd Plagiarism-Detector-C
    ```

2.  **Place input files:** Ensure your `file1.txt`, `file2.txt`, and `hashmap.txt` are in the same directory as the source code (or where you compile your executable).

3.  **Compile the code:**

    ```bash
    gcc -o plagiarism_checker main.c -lm
    ```

    The `-lm` flag is essential for linking the math library (for `sqrt`).

### Usage

Run the compiled executable:

```bash
./plagiarism_checker
```
The program will print the analysis directly to the console.

## ⚙️ Code Structure and Detailed Explanation

The code is organized into several functions, each responsible for a specific stage of the plagiarism detection process.

### Global Constants and Definitions

| **Constant** | **Value** | **Description** |
| :-------------------- | :---------- | :----------------------------------------------------------------------------- |
| `MAX_LINE_LENGTH`     | `1024`      | Maximum length of a line when reading files.                                   |
| `MAX_WORD_LENGTH`     | `100`       | Maximum length for a single word.                                              |
| `INITIAL_BUFFER_SIZE` | `50`        | Initial buffer size for the stemmer.                                           |
| `BUFFER_INCREMENT`    | `50`        | Increment for stemmer buffer reallocation.                                     |
| `MAX_SYNONYM_LENGTH`  | `500`       | Maximum length for a synonym entry in `hashmap.txt`.                           |
| `HASH_MAP_SIZE`       | `50021`     | Size of the hash map for synonym lookup (a prime number for better distribution). |

### Core Functions

Here's a breakdown of the key functions and their roles:

#### 1. `max(int a, int b)`

* **Purpose:** A simple helper function to return the maximum of two integers.
* **Location:** Utility function.
* **Input:** Two integers, `a` and `b`.
* **Output:** The larger of the two integers.

#### 2. `char* read_file(const char* filename)`

* **Purpose:** Reads the entire content of a specified file into a dynamically allocated string. It replaces newline characters with spaces to create a continuous text block.
* **Location:** File I/O.
* **Input:** `filename` (path to the file).
* **Output:** A pointer to the dynamically allocated string containing the file content, or `NULL` on error.
* **Process:** Opens the file, reads it line by line, reallocates memory as needed, and concatenates lines.

#### 3. `char* clean_string(const char *in)`

* **Purpose:** Filters out all non-alphanumeric characters and special symbols from an input string, preserving only letters, numbers, and single spaces. It also handles multiple spaces by converting them to single spaces.
* **Location:** Text Preprocessing.
* **Input:** `in` (the string to clean).
* **Output:** A dynamically allocated string with only alphanumeric characters and single spaces.
* **Process:** Iterates through the input string, checking `isalnum()` for each character.

#### 4. `char* convert_to_lowercase(const char *in)`

* **Purpose:** Converts all uppercase letters in an input string to lowercase.
* **Location:** Text Preprocessing.
* **Input:** `in` (the string to convert).
* **Output:** A dynamically allocated string with all characters in lowercase.
* **Process:** Iterates through the input string, applying `tolower()` to each character.

#### 5. `int count_words_in_string(const char *input)`

* **Purpose:** Counts the number of words in a given string. A word is defined as a sequence of alphanumeric characters separated by non-alphanumeric characters.
* **Location:** Text Analysis Utility.
* **Input:** `input` (the string to count words from).
* **Output:** The total number of words.

#### 6. `char** string_to_array(const char *input, int n)`

* **Purpose:** Splits a processed string into an array of individual word strings.
* **Location:** Text Preprocessing.
* **Input:** `input` (the string to split), `n` (the expected number of words, usually obtained from `count_words_in_string`).
* **Output:** A dynamically allocated array of string pointers, where each pointer points to a word.
* **Process:** Tokenizes the string based on non-alphanumeric delimiters.

#### 7. `int is_stopword(const char *word)` & `char* remove_stopwords_string(const char *input)`

* **Purpose:** `is_stopword` checks if a given `word` is present in a predefined list of common English stopwords. `remove_stopwords_string` then uses this to filter out stopwords from a text.
* **Location:** Text Preprocessing.
* **Input:** `word` (for `is_stopword`), `input` string (for `remove_stopwords_string`).
* **Output:** `1` if it's a stopword, `0` otherwise (for `is_stopword`). A dynamically allocated string with stopwords removed (for `remove_stopwords_string`).
* **Process:** Iterates through a fixed array of stopwords for comparison.

#### 8. `int count_words_in_array(char **words)`

* **Purpose:** Counts the number of non-`NULL` strings in a `NULL`-terminated array of strings.
* **Location:** Array Utility.
* **Input:** `words` (the array of strings).
* **Output:** The count of words in the array.

#### 9. Synonym Replacement (Hash Map Implementation)

This section implements a hash map for efficient synonym lookup, allowing the program to treat different words with similar meanings as the same canonical form.

* **`SynonymNode` Struct:** Represents a node in the hash map's linked list, storing the original word and its canonical synonym.
* **`synonym_hash_map[HASH_MAP_SIZE]`:** A global array of `SynonymNode` pointers, serving as the hash table.
* **`unsigned int hash_function(const char *word)`:** Implements the `djb2` hash function to compute an index for a given word.
* **`void insert_synonym(const char* word, const char* canonical)`:** Inserts a word and its canonical synonym into the hash map. Handles collisions using separate chaining.
* **`void load_synonyms(const char *filename)`:** Reads synonyms from a specified file (e.g., `hashmap.txt`) and populates the hash map. Each line in the file should be in the format `word:synonym1,synonym2,...`. The first synonym after the colon is taken as the canonical form.
* **`const char* get_common_synonym(const char* word)`:** Looks up a word in the hash map and returns its canonical synonym. If not found, it returns the original word.
* **`void free_hash_map()`:** Frees all memory allocated for the synonym hash map.

#### 10. Porter Stemmer (Stemming Algorithm)

This component implements the Porter Stemming algorithm, which reduces words to their root or stem form (e.g., "running" becomes "run", "beautifully" becomes "beauti"). This helps in comparing words with different suffixes as the same base word.

* **`struct stemmer`:** Stores the buffer (`b`), current word length (`k`), start of the stem (`k0`), and suffix start (`j`) for stemming operations.
* **`create_stemmer(void)` & `free_stemmer(struct stemmer *z)`:** Functions for allocating and deallocating stemmer resources.
* **`static int resize_buffer(struct stemmer *z, int new_size)`:** Handles dynamic resizing of the internal buffer used by the stemmer.
* **`static int cons(struct stemmer *z, int i)`:** Checks if the character at index `i` is a consonant.
* **`static int m(struct stemmer *z)`:** Calculates the measure `m` (number of consonant sequences) for a word's stem, crucial for applying stemming rules.
* **`static int vowelinstem(struct stemmer *z)`:** Checks if there is a vowel in the current stem.
* **`static int doublec(struct stemmer *z, int j)`:** Checks for a double consonant at index `j`.
* **`static int cvc(struct stemmer *z, int i)`:** Checks for a CVC (Consonant-Vowel-Consonant) pattern at index `i`.
* **`static int ends(struct stemmer *z, const char *s, int len)`:** Checks if the word ends with a specific suffix `s` of `len` length.
* **`static void setto(struct stemmer *z, const char *s, int len)`:** Replaces the current suffix with a new string `s`.
* **`static void r(struct stemmer *z, const char *s, int len)`:** Applies the replacement `s` if `m > 0`.
* **`static void step1ab(struct stemmer *z)`, `step1c(struct stemmer *z)`, `step2(struct stemmer *z)`, `step3(struct stemmer *z)`, `step4(struct stemmer *z)`, `step5(struct stemmer *z)`:** These functions implement the five steps of the Porter Stemming algorithm, applying specific rules to remove suffixes.
* **`int stem(struct stemmer *z, const char *word, int len)`:** The main stemming function that orchestrates the application of the Porter Stemming steps to a given word.
* **`char *stem_word(const char *word)`:** A wrapper function to simplify the use of the stemmer for a single word, handling memory allocation for the result.

#### 11. `char** find_common_strings(char **arr1, int size1, char **arr2, int size2, int *common_count)`

* **Purpose:** Identifies and returns an array of unique words that are common to both input word arrays.
* **Location:** Similarity Calculation Utility.
* **Input:** Two word arrays (`arr1`, `arr2`) and their respective sizes (`size1`, `size2`).
* **Output:** A dynamically allocated array of common words, and an integer pointer `common_count` holding the count of common words.

#### 12. `char** find_uncommon_strings(char **arr1, int size1, char **arr2, int size2, int *uncommon_count)`

* **Purpose:** Identifies and returns an array of unique words that are present in either `arr1` or `arr2` but not in both.
* **Location:** Similarity Calculation Utility.
* **Input:** Two word arrays (`arr1`, `arr2`) and their respective sizes (`size1`, `size2`).
* **Output:** A dynamically allocated array of uncommon words, and an integer pointer `uncommon_count` holding the count of uncommon words.

#### 13. `int contains(char **arr, int size, const char *word)` & `char** union_arrays(char **arr1, int size1, char **arr2, int size2, int *union_count)`

* **Purpose:** `contains` checks if a word exists in an array. `union_arrays` computes the union of words from two input arrays (i.e., all unique words from both files).
* **Location:** Set Operations Utility.
* **Input:** Word arrays and their sizes.
* **Output:** `1` if contained, `0` otherwise (for `contains`). A dynamically allocated array representing the union, and an integer pointer `union_count` (for `union_arrays`).

#### 14. `double* calculate_tfidf(char **doc1, int rows1, char **doc2, int rows2, char **union_arr, int rows_union, int *result_size)`

* **Purpose:** Calculates the Term Frequency-Inverse Document Frequency (TF-IDF) scores for each word in the `union_arr` relative to the two documents. TF-IDF is a numerical statistic reflecting how important a word is to a document in a collection.
* **Location:** TF-IDF Calculation.
* **Input:** Word arrays for two documents (`doc1`, `doc2`), their sizes, the union of words, and its size.
* **Output:** A dynamically allocated array of TF-IDF scores for the words in the union array, and an integer pointer `result_size`.
* **TF Calculation:** `(count of word in document) / (total words in document)`
* **IDF Heuristic:**
    * If a word appears in only one document (and not the other), its IDF is `3` (higher importance).
    * If a word appears in both documents, its IDF is `2` (lower importance).

#### 15. `double cosine_similarity(double *A, double *B, int size)`

* **Purpose:** Calculates the cosine similarity between two TF-IDF vectors (representing the documents). Cosine similarity measures the cosine of the angle between two non-zero vectors. A value of 1 means identical, 0 means no similarity, and -1 means completely opposite.
* **Location:** Similarity Metric.
* **Input:** Two TF-IDF vectors (`A`, `B`) and their `size`.
* **Output:** The cosine similarity score.
* **Formula:** $\text{similarity} = \frac{A \cdot B}{\|A\| \|B\|}$

#### 16. `int levenshtein_distance(const char *str1, const char *str2)`

* **Purpose:** Calculates the Levenshtein distance (edit distance) between two strings. This is the minimum number of single-character edits (insertions, deletions, or substitutions) required to change one word into the other.
* **Location:** Similarity Metric.
* **Input:** Two strings (`str1`, `str2`).
* **Output:** The Levenshtein distance.

#### 17. `double jaccard_similarity(int a, int b)`

* **Purpose:** Calculates the Jaccard similarity coefficient between two sets. In this context, it's the ratio of the size of the intersection of two sets of words to the size of their union.
* **Location:** Similarity Metric.
* **Input:** `a` (size of common words), `b` (size of union words).
* **Output:** The Jaccard similarity score.
* **Formula:** $\text{similarity} = \frac{|\text{Intersection}|}{|\text{Union}|}$

#### 18. `main()` Function

The `main` function orchestrates the entire plagiarism detection process.

* **Initialization:**
    * Calls `load_synonyms()` to prepare the synonym hash map.
    * Defines the paths for `file1.txt` and `file2.txt`.
* **File Reading:**
    * Reads the contents of `file1.txt` and `file2.txt` using `read_file()`.
* **Preprocessing Pipeline:**
    * Applies `clean_string()` to remove punctuation.
    * Applies `convert_to_lowercase()` to standardize case.
    * Applies `remove_stopwords_string()` to eliminate common words.
    * Uses `count_words_in_string()` to get word counts.
    * Converts the processed strings into word arrays using `string_to_array()`.
    * Replaces synonyms in the word arrays using `get_common_synonym()`.
    * Applies stemming using `stem_word()` to the synonym-replaced words.
* **Similarity Calculation:**
    * Determines `common` words and `uncommon` words between the stemmed word arrays.
    * Calculates the `union_array` of all unique words from both stemmed documents.
    * Computes TF-IDF vectors for both files based on the `union_array` using `calculate_tfidf()`.
    * Calculates `cosine_similarity` using the TF-IDF vectors.
    * Calculates `levenshtein_distance` between the preprocessed (cleaned, lowercased, stopword-removed) strings.
    * Calculates `jaccard_similarity` using the common and union word counts.
* **Final Plagiarism Percentage:**
    * Combines the three similarity scores with weighted averages:
        * Cosine Similarity: 30% weight
        * Levenshtein Distance: 45% weight
        * Jaccard Similarity: 25% weight
    * **Formula:** $\text{Plagiarism} = 100 \times (\text{Cosine} \times 0.30 + \text{Levenshtein} \times 0.45 + \text{Jaccard} \times 0.25)$
* **Output:**
    * Prints unique words found in `file1` and `file2` (before stemming and synonym replacement, for better readability).
    * Prints common words found between `file1` and `file2` (before stemming and synonym replacement).
    * Displays the total counts for unique and common words.
    * Outputs the final calculated plagiarism percentage.
    * Provides a simple verdict: "Possible Plagiarism Detected." if `final_percentage > 50.00%`, otherwise "Documents appear sufficiently different."
* **Memory Deallocation:**
    * Frees all dynamically allocated memory to prevent memory leaks.
## 🤝 Contributing

Contributions are welcome! If you find any bugs or have suggestions for improvements, please open an issue or submit a pull request.

## 📄 License

This project is open-source.

## 📞 Contact

For any queries or feedback, please feel free to reach out at hrik434@gmail.com.

---


