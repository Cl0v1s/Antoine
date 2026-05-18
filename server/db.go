package main

import (
	"database/sql"
	"fmt"

	_ "github.com/mattn/go-sqlite3"
)

var db *sql.DB

func InitDb() error {
	var err error
	db, err = sql.Open("sqlite3", "./antoine.db")
	if err != nil {
		return err
	}
	return populateDb()
}

func populateDb() error {
	if db == nil {
		return fmt.Errorf("You must call initDb first")
	}
	sqlStmt := `
		create table if not exists bottles (id integer not null primary key, recipientName string not null, recipientTownName string not null, senderName string not null, senderTownName string not null, body string not null);
	`
	_, err := db.Exec(sqlStmt)
	if err != nil {
		return err
	}
	return nil
}

func ThrowBottle(recipient *Recipient, bottle *Letter) error {
	if db == nil {
		return fmt.Errorf("You must call initDb first")
	}
	stmt, err := db.Prepare(`
		insert into bottles (recipientName, recipientTownName, senderName, senderTownName, body) values (?, ?, ?, ?, ?);
	
	`)
	if err != nil {
		return err
	}
	defer stmt.Close()
	_, err = stmt.Exec(recipient.Name, recipient.TownName, bottle.PlayerName, bottle.TownName, bottle.Body)
	return err
}

func FindBottle(recipientName string, recipientTownName string) (*Letter, error) {
	if db == nil {
		return nil, fmt.Errorf("You must call initDb first")
	}
	stmt, err := db.Prepare(`select id, senderName, senderTownName, body from bottles where recipientName = ? and recipientTownName = ?;`)
	if err != nil {
		return nil, err
	}
	defer stmt.Close()

	row := stmt.QueryRow(recipientName, recipientTownName)
	var id int
	var senderName, senderTownName, body string
	err = row.Scan(&id, &senderName, &senderTownName, &body)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	if err != nil {
		return nil, err
	}
	stmt, err = db.Prepare(`delete from bottles where id = ?;`)
	if err != nil {
		return nil, err
	}
	defer stmt.Close()
	_, err = stmt.Exec(id)
	if err != nil {
		return nil, err
	}
	bottle := Letter{
		Intro: "Pour " + recipientName + " de " + recipientTownName, // TODO intl,
		Body:  body,
		End:   "Signé " + senderName + " de " + senderTownName,
	}

	return &bottle, nil
}
