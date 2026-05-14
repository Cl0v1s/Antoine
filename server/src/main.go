package main

import (
	"fmt"
	"os"
	"strings"
	"unicode/utf8"

	"github.com/gin-gonic/gin"
	"golang.org/x/text/encoding/charmap"
)

func sanitize(s string) string {
	s = strings.ReplaceAll(s, "**", "")
	s = strings.ReplaceAll(s, "*", "")
	r := strings.NewReplacer(
		"…", "...",
		"‘", "'",
		"’", "'",
		"“", "\"",
		"”", "\"",
		"–", "-",
		"—", "-",
		"«", "\"",
		"»", "\"",
	)
	s = r.Replace(s)
	// drop any remaining rune outside ISO-8859-1 (emojis, etc.)
	var b strings.Builder
	for _, c := range s {
		if c <= 0xFF {
			b.WriteRune(c)
		}
	}
	return b.String()
}

func buildPrompt(language string, score int, receiverName string, senderDescription string, townName string, attachmentName string, content string) (string, error) {
	var tone string
	if score > 80 {
		tone = "very happy"
	} else if score > 50 {
		tone = ""
	} else {
		tone = "confused or negative"
	}

	promptTemplate, err := os.ReadFile("prompt.txt")
	if err != nil {
		return "", err
	}

	return fmt.Sprintf(string(promptTemplate), townName, senderDescription, receiverName, attachmentName, content, tone, language), nil
}

type GenRequest struct {
	Language     string `json:"language"`
	SenderId     string `json:"senderId"`     // villager id 002d
	ReceiverName string `json:"receiverName"` // receiver (player?) name
	TownName     string `json:"townName"`     // town name
	AttachmentId uint16 `json:"attachmentId"` // id of the attached gift
	Score        int    `json:"score"`        // < 50 -> negative or confused reply
	Intro        string `json:"intro"`        // previous letter from the receiver
	Body         string `json:"body"`         // previous letter from the receiver
	End          string `json:"end"`          // previous letter from the receiver
}

func gen(c *gin.Context) {
	var request GenRequest
	err := c.BindJSON(&request)
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}

	fmt.Printf("Request: %+v\n", request)

	senderName, exists := GetVillagerName(request.SenderId)
	if !exists {
		c.String(404, "Villager "+request.SenderId+" does not exists.")
		return
	}

	senderDescription, err := LoadVillagerInfos(senderName)
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}

	attachmentName := "Package"
	if request.AttachmentId == 0xf1ff {
		attachmentName = "No package"
	}

	prompt, err := buildPrompt(request.Language, request.Score, request.ReceiverName, *senderDescription, request.TownName, attachmentName, request.Intro+"\n"+request.Body+"\n"+request.End)
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}
	if gin.IsDebugging() {
		fmt.Println(prompt)
	}
	response, err := Call(prompt)
	if gin.IsDebugging() {
		fmt.Println(response)
	}
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
	} else {
		reply := sanitize(response.Choices[0].Message.Content)

		// we cant trust llms regarding response length
		parts := strings.Split(reply, "\n\n")
		body := parts[1]
		var punctuation []rune
		for _, r := range body {
			if r == '.' || r == '?' || r == '!' {
				punctuation = append(punctuation, r)
			}
		}
		for utf8.RuneCountInString(body) > 96 {
			bodyparts := strings.FieldsFunc(body, func(r rune) bool {
				return r == '.' || r == '?' || r == '!'
			})
			bodyparts = bodyparts[:len(bodyparts)-1]
			body = ""
			for i := 0; i < len(bodyparts); i++ {
				body += bodyparts[i] + string(punctuation[i])
			}
		}
		parts[1] = body

		reply = strings.Join(parts, "\n\n")

		encoder := charmap.ISO8859_1.NewEncoder()
		iso8859Text, err := encoder.String(reply)
		if err != nil {
			fmt.Println(err)
			c.String(500, err.Error())
			return
		}
		c.Header("Content-Type", "text/plain; charset=ISO-8859-1")
		c.String(200, iso8859Text)
	}
}

func main() {
	router := gin.Default()
	router.GET("/gen", gen)
	router.Run() // listen and serve on 0.0.0.0:8080
}
