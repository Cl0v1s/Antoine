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
		create unique index if not exists bottles_unique on bottles(recipientName, recipientTownName, senderName, senderTownName, body);
	`
	_, err := db.Exec(sqlStmt)
	if err != nil {
		return err
	}
	return nil
}
