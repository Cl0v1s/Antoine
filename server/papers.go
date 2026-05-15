package main

import "math/rand"

type Paper struct {
	ID   uint16
	Name string
}

var Papers = []Paper{
	{ID: 0x01, Name: "Butterfly"},
	{ID: 0x02, Name: "Airmail"},
	{ID: 0x03, Name: "New"},
	{ID: 0x04, Name: "Lacy"},
	{ID: 0x05, Name: "Cloudy"},
	{ID: 0x06, Name: "Petal"},
	{ID: 0x07, Name: "Snowy"},
	{ID: 0x08, Name: "Maple-leaf"},
	{ID: 0x09, Name: "Lined"},
	{ID: 0x0a, Name: "Notebook"},
	{ID: 0x0b, Name: "Flowery"},
	{ID: 0x0c, Name: "Polka-dot"},
	{ID: 0x0d, Name: "Ribbon"},
	{ID: 0x0e, Name: "Sparkly"},
	{ID: 0x0f, Name: "Vine"},
	{ID: 0x10, Name: "Formal"},
	{ID: 0x11, Name: "Snowman"},
	{ID: 0x12, Name: "Card"},
	{ID: 0x13, Name: "Leopard"},
	{ID: 0x14, Name: "Cow"},
	{ID: 0x15, Name: "Camouflage"},
	{ID: 0x16, Name: "Hamburger"},
	{ID: 0x17, Name: "Piano"},
	{ID: 0x18, Name: "Nook"},
	{ID: 0x19, Name: "Fox"},
	{ID: 0x1a, Name: "Birthday"},
	{ID: 0x1b, Name: "Four-leaf"},
	{ID: 0x1c, Name: "Town-hall"},
	{ID: 0x1d, Name: "Tortimer"},
	{ID: 0x1e, Name: "Insurance"},
	{ID: 0x1f, Name: "Academy"},
	{ID: 0x20, Name: "Lovely"},
	{ID: 0x21, Name: "Rainbow"},
	{ID: 0x22, Name: "Egyptian"},
	{ID: 0x23, Name: "Lotus"},
	{ID: 0x24, Name: "Tile"},
	{ID: 0x25, Name: "Mosaic"},
	{ID: 0x26, Name: "Elegant"},
	{ID: 0x27, Name: "Town"},
	{ID: 0x28, Name: "Chinese"},
	{ID: 0x29, Name: "Ocean"},
	{ID: 0x2a, Name: "Industrial"},
	{ID: 0x2b, Name: "Fireworks"},
	{ID: 0x2c, Name: "Floral"},
	{ID: 0x2d, Name: "Mushroom"},
	{ID: 0x2e, Name: "Star"},
	{ID: 0x2f, Name: "Composer"},
	{ID: 0x30, Name: "Bathtub"},
	{ID: 0x31, Name: "SMB3"},
	{ID: 0x32, Name: "Cool"},
	{ID: 0x33, Name: "Forest"},
	{ID: 0x34, Name: "Bubble"},
	{ID: 0x35, Name: "Buttercup"},
	{ID: 0x36, Name: "Tartan"},
	{ID: 0x37, Name: "Plaid"},
	{ID: 0x38, Name: "Lemon-lime"},
	{ID: 0x39, Name: "Crater"},
	{ID: 0x3a, Name: "Bejeweled"},
	{ID: 0x3b, Name: "Geometric"},
	{ID: 0x3c, Name: "Southwest"},
	{ID: 0x3d, Name: "Night"},
	{ID: 0x3e, Name: "Chic"},
	{ID: 0x3f, Name: "Goldfish"},
}

func randomPaper() uint16 {
	index := rand.Intn(len(Papers))
	return Papers[index].ID
}
