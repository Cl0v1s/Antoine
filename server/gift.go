package main

import (
	"math/rand"
)

type LetterType int

const (
	SHORT  LetterType = iota
	MEDIUM LetterType = iota
	LONG   LetterType = iota
)

var (
	giftShirts     []uint16
	giftFruits     = []uint16{0x1518, 0x1519, 0x151a, 0x151b, 0x151c, 0x1548} // Apple, Orange, Pear, Peach, Cherry, Coconut
	giftFurnitures []uint16
	giftCarpets    []uint16
	giftWallpapers []uint16
)

// this is ran automatically on startup
func init() {
	for id := range items {
		switch {
		case id >= 0x11A8 && id <= 0x12AF:
			giftShirts = append(giftShirts, id)
		case id >= 0x3000 && id <= 0x45D8:
			giftFurnitures = append(giftFurnitures, id)
		case id >= 0x1100 && id <= 0x1143:
			giftWallpapers = append(giftWallpapers, id)
		case id >= 0x1144 && id <= 0x1187:
			giftCarpets = append(giftCarpets, id)
		}
	}
}

func getItemType(id uint16) string {
	if id >= 0x01 && id <= 0x1c {
		return "FLOWER"
	} else if id >= 0x1D && id <= 0x24 {
		return "WEED"
	} else if (id >= 0x25 && id <= 0x6D) || (id >= 0xC7 && id <= 0xD3) {
		return "TREE"
	} else if id >= 0x6E && id <= 0x89 {
		return "PARCHEDFLOWER"
	} else if id >= 0x8A && id <= 0xA5 {
		return "WATEREDFLOWER"
	} else if id >= 0xA7 && id <= 0xC6 {
		return "FABRICPATTERN"
	} else if (id >= 0xE3 && id <= 0xE7) || (id >= 0xED && id <= 0xFB) {
		return "ROCK"
	} else if id >= 0xE8 && id <= 0xEC {
		return "MONEYROCK"
	} else if id >= 0x1000 && id <= 0x10FF {
		return "PAPER"
	} else if (id >= 0x1100 && id <= 0x1143) || (id >= 0x1144 && id <= 0x1187) {
		return "WALLPAPER/CARPET"
	} else if id >= 0x11A8 && id <= 0x12AF {
		return "SHIRT"
	} else if id >= 0x12B0 && id <= 0x1322 {
		return "CATCHABLE"
	} else if id >= 0x1323 && id <= 0x1368 {
		return "SONG"
	} else if id >= 0x1369 && id <= 0x139F {
		return "TOOL"
	} else if id >= 0x13A8 && id <= 0x1457 {
		return "HATS/GLASSES/MASKS"
	} else if id >= 0x1492 && id <= 0x14FD {
		return "MONEY"
	} else if (id >= 0x14FE && id <= 0x1530) || (id >= 0x1542 && id <= 0x1548) || (id >= 0x155E && id <= 0x156D) {
		return "ITEM"
	} else if id >= 0x1531 && id <= 0x1541 {
		return "TURNIP"
	} else if id == 0x1549 {
		return "FOSSIL"
	} else if id >= 0x1554 && id <= 0x155C {
		return "SHELL"
	} else if id >= 0x3000 && id <= 0x45D8 {
		return "FURNITURE"
	} else if id >= 0x45DC && id <= 0x47D4 {
		return "GYROID"
	} else if id >= 0x47D8 && id <= 0x4A44 {
		return "PIC"
	}
	return ""
}

func pickFrom(pools ...[]uint16) uint16 {
	total := 0
	for _, p := range pools {
		total += len(p)
	}
	n := rand.Intn(total)
	for _, p := range pools {
		if n < len(p) {
			return p[n]
		}
		n -= len(p)
	}
	return 0
}

func randomGift(lt LetterType) uint16 {
	switch lt {
	case SHORT:
		return pickFrom(giftShirts, giftFruits)
	case MEDIUM:
		return pickFrom(giftFurnitures)
	case LONG:
		return pickFrom(giftFurnitures, giftCarpets, giftWallpapers)
	}
	return 0
}

// Please see https://www.thonky.com/acww/letter-writing-tips
func GenerateAnswerGift(request Letter, score int) uint16 {
	// We only return a gift for a well written letter with an attachment
	if score <= 50 || request.AttachmentId == NO_ITEM || request.AttachmentId == 0 {
		return 0xFFF1
	}
	// 26 letters per lines
	lines := len(request.Body) / 26
	var lt LetterType
	if lines < 2 {
		lt = SHORT
	} else if lines == 2 {
		lt = MEDIUM
	} else {
		lt = LONG
	}
	return randomGift(lt)
}
