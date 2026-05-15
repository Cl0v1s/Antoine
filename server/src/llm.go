package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"strings"
	"time"
	"unicode/utf8"

	"github.com/gin-gonic/gin"
)

// Définition des types Go pour la réponse de l'API
type Message struct {
	Role    string `json:"role"`
	Content string `json:"content"`
}

type Choice struct {
	Index        int     `json:"index"`
	Message      Message `json:"message"`
	FinishReason string  `json:"finish_reason"`
}

type Usage struct {
	PromptTokens     int `json:"prompt_tokens"`
	CompletionTokens int `json:"completion_tokens"`
	TotalTokens      int `json:"total_tokens"`
}

type APIResponse struct {
	ID      string   `json:"id"`
	Object  string   `json:"object"`
	Created int64    `json:"created"`
	Model   string   `json:"model"`
	Choices []Choice `json:"choices"`
	Usage   Usage    `json:"usage"`
}

func interpolate(template string, vars map[string]string) string {
	for k, v := range vars {
		template = strings.ReplaceAll(template, "{{"+k+"}}", v)
	}
	return template
}

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

func buildPrompt(language string, score int, receiverName string, senderDescription string, townName string, receivedAttachmentName string, sentAttachmentName string, content string) (string, error) {
	var tone string
	if score > 80 {
		tone = "very happy"
	} else if score > 50 {
		tone = "neutral"
	} else {
		tone = "confused or negative"
	}

	promptTemplate, err := os.ReadFile("prompt.txt")
	if err != nil {
		return "", err
	}

	receivedAttachmentLine := ""
	if receivedAttachmentName != "" {
		receivedAttachmentLine = " with " + receivedAttachmentName + " attached"
	}

	sendtAttachmentLine := ""
	if sentAttachmentName != "" {
		sendtAttachmentLine = "; naturally mention that you are sending " + sentAttachmentName
	}

	return interpolate(string(promptTemplate), map[string]string{
		"townName":               townName,
		"traits":                 senderDescription,
		"receiverName":           receiverName,
		"receivedAttachmentLine": receivedAttachmentLine,
		"sendtAttachmentLine":    sendtAttachmentLine,
		"letter":                 content,
		"tone":                   tone,
		"language":               language,
	}), nil
}

func getAttachmentName(id uint16) string {
	if gin.IsDebugging() {
		fmt.Printf("Letter contains gift %d\n", id)
	}
	if id == NO_ITEM || id == 0 {
		return ""
	}

	name, ok := items[id]
	if !ok {
		return "a gift"
	}

	return name + "(" + getItemType(id) + ")"
}

func GenerateAnswerContent(request Letter, senderName *string, giftId uint16) (*AnswerContent, error) {
	senderDescription, err := LoadVillagerInfos(*senderName)
	if err != nil {
		return nil, err
	}
	sentAttachmentName := getAttachmentName(giftId)
	receivedAttachmentName := getAttachmentName(request.AttachmentId)
	prompt, err := buildPrompt(request.Language, request.Score, request.PlayerName, *senderDescription, request.TownName, receivedAttachmentName, sentAttachmentName, request.Intro+"\n"+request.Body+"\n"+request.End)
	if err != nil {
		return nil, err
	}

	if gin.IsDebugging() {
		fmt.Println("Prompt: " + prompt)
	}
	response, err := call(prompt)
	if gin.IsDebugging() {
		fmt.Println("Response: ")
		fmt.Println(response)
	}

	if err != nil {
		return nil, err
	}

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

	return &AnswerContent{
		Intro: parts[0],
		Body:  parts[1],
		End:   parts[2],
	}, nil
}

func call(prompt string) (*APIResponse, error) {
	url := "https://api.mistral.ai/v1/chat/completions"
	apiKey := os.Getenv("API_KEY")
	data := map[string]interface{}{
		"model":      "mistral-small-latest",
		"max_tokens": utf8.RuneCountInString(prompt) + 129, // 129 is the max length of a letter in the game
		"messages": []map[string]string{
			{"role": "user", "content": prompt},
		},
	}

	jsonData, _ := json.Marshal(data)
	req, _ := http.NewRequest("POST", url, bytes.NewBuffer(jsonData))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Accept", "application/json")
	req.Header.Set("Authorization", "Bearer "+apiKey)

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		fmt.Println("Error:", err)
		return nil, err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("non-200 status code: %d", resp.StatusCode)
	}

	time.Sleep(2 * time.Second) // we dont want to spam them

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil, err
	}

	var response APIResponse
	err = json.Unmarshal(body, &response)
	if err != nil {
		return nil, err
	}

	return &response, nil
}
