package main

import (
	"encoding/json"
	"fmt"
	"log"
	"strings"

	"github.com/gin-gonic/gin"
	"golang.org/x/text/encoding/charmap"
)

type AnswerContent struct {
	Intro string
	Body  string
	End   string
}

type Letter struct {
	Language     string `json:"language"`
	VillagerId   string `json:"villagerId"`   // villager id 002d
	PlayerName   string `json:"playerName"`   // player name
	TownName     string `json:"townName"`     // town name
	AttachmentId uint16 `json:"attachmentId"` // id of the attached gift
	Intro        string `json:"intro"`
	Body         string `json:"body"`
	End          string `json:"end"`
	PaperId      uint16 `json:"paperId"`
	Score        int    `json:"score"`
}

func gen(c *gin.Context) {
	var request Letter
	err := c.BindJSON(&request)
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}

	fmt.Printf("Request: %+v\n", request)

	senderName, exists := GetVillagerName(request.VillagerId)
	if !exists {
		c.String(404, "Villager "+request.VillagerId+" does not exists.")
		return
	}

	score := calculateScore(request.Language, request.Body)

	giftId := GenerateAnswerGift(request, score)

	answer, err := GenerateAnswerContent(request, &senderName, giftId, score)
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}

	jsonBytes, err := json.Marshal(Letter{
		Intro:        answer.Intro,
		Body:         answer.Body,
		End:          answer.End,
		AttachmentId: giftId,
		PaperId:      randomPaper(),
		Score:        score,
	})
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}

	encoder := charmap.ISO8859_1.NewEncoder()
	iso8859JSON, err := encoder.String(string(jsonBytes))
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}
	c.Header("Content-Type", "application/json; charset=ISO-8859-1")
	c.String(200, iso8859JSON)
}

func throw(c *gin.Context) {
	var request Letter
	err := c.BindJSON(&request)
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}

	fmt.Printf("Request: %+v\n", request)
	if len(strings.Trim(request.Body, " ")) == 0 {
		c.String(400, "Body can not be empty")
		return
	}
	if len(strings.Trim(request.End, " ")) == 0 {
		c.String(400, "End can not be empty")
		return
	}
	if len(strings.Trim(request.PlayerName, " ")) == 0 {
		c.String(400, "PlayerName can not be empty")
		return
	}
	if len(strings.Trim(request.TownName, " ")) == 0 {
		c.String(400, "TownName can not be empty")
		return
	}

	recipient, err := ExtractRecipient(request.End)
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}
	err = ThrowBottle(recipient, &request)
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}
	c.Status(201)
}

func find(c *gin.Context) {
	var request Letter
	err := c.BindJSON(&request)
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}
	fmt.Printf("Request: %+v\n", request)
	if len(strings.Trim(request.PlayerName, " ")) == 0 {
		c.String(400, "PlayerName can not be empty")
		return
	}
	if len(strings.Trim(request.TownName, " ")) == 0 {
		c.String(400, "TownName can not be empty")
		return
	}
	bottle, err := FindBottle(request.PlayerName, request.TownName)
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}
	if bottle == nil {
		c.Status(404)
		return
	}
	jsonBytes, err := json.Marshal(Letter{
		Intro: bottle.Intro,
		Body:  bottle.Body,
		End:   bottle.End,
	})
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}
	encoder := charmap.ISO8859_1.NewEncoder()
	iso8859JSON, err := encoder.String(string(jsonBytes))
	if err != nil {
		fmt.Println(err)
		c.String(500, err.Error())
		return
	}
	c.Header("Content-Type", "application/json; charset=ISO-8859-1")
	c.String(200, iso8859JSON)
}

func main() {
	err := InitDb()
	if err != nil {
		log.Fatal(err)
	}
	defer db.Close()
	router := gin.Default()
	router.POST("/gen", gen)
	router.POST("/throw", throw)
	router.POST("/find", find)
	router.Run() // listen and serve on 0.0.0.0:8080
}
