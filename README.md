# Plagiarism Detector in C

This project compares two text files and detects plagiarism using multiple Natural Language Processing techniques and similarity algorithms.

## 💡 Features

- Cleans and normalizes text
- Removes stopwords
- Converts to lowercase
- Applies stemming (Porter Stemmer)
- Replaces synonyms using a hash map
- Calculates:
  - Cosine Similarity with TF-IDF
  - Levenshtein Distance
  - Jaccard Similarity
- Displays similarity scores and final plagiarism percentage

## 🗂️ Files

| File         | Description                       |
|--------------|-----------------------------------|
| `main.c`     | Main source code (was code.txt)   |
| `hashmap.txt`| Synonym mapping file              |
| `file1.txt`  | Input file 1 for comparison       |
| `file2.txt`  | Input file 2 for comparison       |
| `README.md`  | Project overview and instructions |

## 🚀 Usage

1. Place your text files and synonym file in the same folder.
2. Compile using:

```bash
gcc -o plagiarism main.c -lm
