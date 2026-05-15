#ifndef SCORE_H
#define SCORE_H

#include <string>
#include <algorithm>

#include "trigrams.h"

// This code implements the work of James Chambers described here:
// https://jamchamb.net/projects/animal-crossing-letters (Was saved on the wayback machine)


#define PONCTUATION_LENGTH 3
const char ponctuation[PONCTUATION_LENGTH] = {
    '.',
    '?',
    '!',
};

static inline bool isPunct(char c) {
    bool found = false;
    int i = 0;
    while(found == false && i < PONCTUATION_LENGTH) {
        if(c == ponctuation[i]) found = true;
        i += 1;
    }
    return found;
}

static inline bool isSpace(char c) {
    return c == ' ';
}

/**
 * A: Checks for punctuation at the end of sentences, and capital letters. 
 * A ‘.’, ‘?’, or ‘!’ symbol at the end of the letter grants 20 points. 
 * After that, every punctuation mark must be followed by a capital letter within 3 spaces (+10 or -10 each occurrence)
 */
static inline int ACheck(int score, std::string body) { // the copy is made on purpose
    if(body.empty()) return score;
    // to simplify things we convert all ponctuation to the same char
    for(int i = 0; i < PONCTUATION_LENGTH; i++) {
        std::replace(body.begin(), body.end(), ponctuation[i], ponctuation[0]);
    }

    if (body.back() == ponctuation[0]) {
        score += 20;
    }

    std::string::iterator pos = body.begin();
    while((pos = std::find(pos + 1, body.end(), ponctuation[0])) != body.end()) {
        if(pos == (body.end() - 1)) continue; // we dont check for the last sentence
        bool foundCapital = false;
        for (int i = 1; i <= 3 && (pos + i) != body.end(); i++) {
            if (std::isalpha(*(pos + i)) && std::isupper(*(pos + i))) {
            if (i == 1 || *(pos + i - 1) == ' ') {
                foundCapital = true;
                break;
            }
            }
        }
        score += foundCapital ? 10 : -10;
    }

    return score;
}

/**
 * B: Checks the beginning of each word for common trigrams, using this lookup table: 
 * https://github.com/jamchamb/ac-letter-score/blob/master/trigram_table.txt 
 * The total number of trigram hits is multiplied by 3 and added to the score.
 */
static inline int BCheck(const std::string &language, int score, std::string &body) {
    const std::vector<std::string>* trigrams;
    // yuck but works for two languages 
    if (std::toupper(language[0]) == 'F' && std::toupper(language[1]) == 'R' &&
        std::toupper(language[2]) == 'E' && std::toupper(language[3]) == 'N' &&
        std::toupper(language[4]) == 'C' && std::toupper(language[5]) == 'H' &&
        language.size() == 6) {
        trigrams = &TRIGRAMS_FRENCH;
    } else {
        trigrams = &TRIGRAMS_ENGLISH;
    }

    int count = 0;

    std::string::iterator pos = body.begin();
    while(pos != body.end()) {
        int index = std::distance(body.begin(), pos);
        std::string part = body.substr(index, 3);
        if(std::find(trigrams->begin(), trigrams->end(), part) != trigrams->end()) {
            count += 1;
        }
        auto nextSpace = std::find(pos + 3, body.end(), ' ');
        pos = std::find_if_not(nextSpace, body.end(), isSpace);
    }
    return score + count * 3;
}

/**
 * C: This checks for the first (non-space) character in the letter. 
 * If it’s a capital letter, gives +20 points. If not, -10 points.
 */

static inline int CCheck(int score, std::string &body) {
    std::string::iterator c = std::find_if_not(body.begin(), body.end(), isSpace);
    if(c == body.end()) return score - 10;
    if(*c == std::toupper(*c)) return score + 20;
    return score - 10;
}

/**
 * D: If a letter of the alphabet is repeated three times in a row, 
 * deducts 50 points and stops checking. Otherwise it does not affect the score.
 */
static inline int DCheck(int score, std::string &body) {
    bool found = false;
    unsigned int i = 0;
    while(found == false && i < body.length()) {
        char c = body[i];
        int count = 1;
        for(int u = 1; u <= 2; u++) {
            if(c == body[i + u]) {
                count += 1;
            } else {
                break;
            }
        }
        if(count >= 3) {
            found = true;
        }
        i += 1;
    }
    if(found) {
        return score - 50;
    }
    return score;
}

/**
 * E: Checks ratio of spaces to non-spaces in the letter. 
 * If it’s all spaces, or n_other/n_space*100 is less than 20, gives -20 points. 
 * Otherwise, +20 points.
 */
static inline int ECheck(int score, std::string &body) {
    int n_spaces = std::count_if(body.begin(), body.end(), isSpace);
    int n_other = body.size() - n_spaces;
    if(n_spaces == 0) n_spaces = 1;

    if (n_other == 0 || (n_other / n_spaces * 100) < 20) {
        return score - 20;
    }
    return score + 20;
}

/**
 * F: If the letter is more than 75 characters long and goes on for at least 75 characters 
 * without any punctuation (., ?, !), it deducts 150 points!
 */
static inline int FCheck(int score, std::string &body) {
    if(body.length() <= 75) return score;
    auto firstPunct = std::find_if(body.begin(), body.end(), isPunct);
    if(std::distance(body.begin(), firstPunct) > 75) return score - 150;
    return score;
}

/**
 * G: Checks each group of 32 characters for at least one space. 
 * Deducts 20 points for each group that doesn’t have one.
 */
static inline int GCheck(int score, std::string &body) {
    int count = 0;
    for(unsigned int i = 0; i < body.length(); i += 32) {
        std::string part = body.substr(i, 32);
        bool found = std::find(part.begin(), part.end(), ' ') != part.end();
        if(found == false) count += 1;
    }
    return score - count * 20;
}

static inline int calculateScore(const std::string &language, std::string body) { // copy is made on purpose
    // Trim leading and trailing spaces and null bytes (body is a fixed-size buffer in game memory)
    const std::string trimChars(" \0", 2);
    size_t start = body.find_first_not_of(trimChars);
    size_t end = body.find_last_not_of(trimChars);
    body = (start == std::string::npos || end == std::string::npos) ? "" : body.substr(start, end - start + 1);

    int score = 0;
    score = ACheck(score, body);
    score = BCheck(language, score, body);
    score = CCheck(score, body);
    score = DCheck(score, body);
    score = ECheck(score, body);
    score = FCheck(score, body);
    score = GCheck(score, body);
    return score;
}

#endif