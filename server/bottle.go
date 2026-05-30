package main

import (
	"database/sql"
	"fmt"
)

func ThrowBottle(recipient *Recipient, bottle *Letter) error {
	if db == nil {
		return fmt.Errorf("You must call initDb first")
	}
	stmt, err := db.Prepare(`
		insert into bottles (recipientName, recipientTownName, senderName, senderTownName, body) values (?, ?, ?, ?, ?) on conflict do nothing;
	
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
