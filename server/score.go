package main

import (
	"strings"
	"unicode"
)

var punctChars = []rune{'.', '?', '!'}

func isPunct(c rune) bool {
	for _, p := range punctChars {
		if c == p {
			return true
		}
	}
	return false
}

// aCheck: punctuation at end of sentences and capital letters after punctuation.
// A '.', '?', or '!' at the end grants +20. Each mid-letter punctuation followed by
// a capital within 3 chars gives +10, otherwise -10.
func aCheck(score int, body string) int {
	if body == "" {
		return score
	}
	runes := []rune(body)
	for i, c := range runes {
		if isPunct(c) {
			runes[i] = '.'
		}
	}
	if runes[len(runes)-1] == '.' {
		score += 20
	}
	for i := 1; i < len(runes); i++ {
		if runes[i] != '.' {
			continue
		}
		if i == len(runes)-1 {
			continue
		}
		foundCapital := false
		for j := 1; j <= 3 && i+j < len(runes); j++ {
			c := runes[i+j]
			if unicode.IsLetter(c) && unicode.IsUpper(c) {
				if j == 1 || runes[i+j-1] == ' ' {
					foundCapital = true
					break
				}
			}
		}
		if foundCapital {
			score += 10
		} else {
			score -= 10
		}
	}
	return score
}

// bCheck: checks the first 3 chars of each "word" against common trigrams.
// The iteration mirrors the C++ original: after taking 3 chars from pos, the next
// word search starts from pos+3, so short words cause the following word to be skipped.
func bCheck(language string, score int, body string) int {
	var trigramList []string
	if strings.EqualFold(language, "french") {
		trigramList = trigramsFrench
	} else {
		trigramList = trigramsEnglish
	}
	trigramSet := make(map[string]bool, len(trigramList))
	for _, t := range trigramList {
		trigramSet[t] = true
	}

	count := 0
	runes := []rune(body)
	pos := 0
	for pos < len(runes) {
		end := pos + 3
		if end > len(runes) {
			end = len(runes)
		}
		if trigramSet[string(runes[pos:end])] {
			count++
		}
		nextSpace := len(runes)
		for i := pos + 3; i < len(runes); i++ {
			if runes[i] == ' ' {
				nextSpace = i
				break
			}
		}
		pos = len(runes)
		for i := nextSpace; i < len(runes); i++ {
			if runes[i] != ' ' {
				pos = i
				break
			}
		}
	}
	return score + count*3
}

// cCheck: +20 if the first non-space character is uppercase (or non-letter), -10 otherwise.
func cCheck(score int, body string) int {
	for _, c := range body {
		if c != ' ' {
			if c == unicode.ToUpper(c) {
				return score + 20
			}
			return score - 10
		}
	}
	return score - 10
}

// dCheck: -50 if any letter is repeated 3+ times in a row.
func dCheck(score int, body string) int {
	runes := []rune(body)
	for i := 0; i < len(runes); i++ {
		count := 1
		for u := 1; u <= 2; u++ {
			if i+u < len(runes) && runes[i] == runes[i+u] {
				count++
			} else {
				break
			}
		}
		if count >= 3 {
			return score - 50
		}
	}
	return score
}

// eCheck: +20 if non-space/space ratio >= 20%, -20 otherwise.
func eCheck(score int, body string) int {
	nSpaces := strings.Count(body, " ")
	nOther := len([]rune(body)) - nSpaces
	if nSpaces == 0 {
		nSpaces = 1
	}
	if nOther == 0 || (nOther/nSpaces*100) < 20 {
		return score - 20
	}
	return score + 20
}

// fCheck: -150 if the letter is over 75 chars and has no punctuation in the first 75 chars.
func fCheck(score int, body string) int {
	runes := []rune(body)
	if len(runes) <= 75 {
		return score
	}
	for i, c := range runes {
		if isPunct(c) {
			if i > 75 {
				return score - 150
			}
			return score
		}
	}
	return score - 150
}

// gCheck: -20 for each group of 32 characters that contains no space.
func gCheck(score int, body string) int {
	runes := []rune(body)
	count := 0
	for i := 0; i < len(runes); i += 32 {
		end := i + 32
		if end > len(runes) {
			end = len(runes)
		}
		hasSpace := false
		for _, c := range runes[i:end] {
			if c == ' ' {
				hasSpace = true
				break
			}
		}
		if !hasSpace {
			count++
		}
	}
	return score - count*20
}

func calculateScore(language string, body string) int {
	body = strings.Trim(body, " \x00")
	score := 0
	score = aCheck(score, body)
	score = bCheck(language, score, body)
	score = cCheck(score, body)
	score = dCheck(score, body)
	score = eCheck(score, body)
	score = fCheck(score, body)
	score = gCheck(score, body)
	return score
}
