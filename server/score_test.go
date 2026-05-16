package main

import (
	"strings"
	"testing"
)

func TestACheck(t *testing.T) {
	if got := aCheck(0, "Hello world."); got != 20 {
		t.Errorf("aCheck(0, \"Hello world.\") = %d, want 20", got)
	}
	c := "Hello world!"
	if got := aCheck(0, c); got != 20 {
		t.Errorf("aCheck(0, \"Hello world!\") = %d, want 20", got)
	}
	if c != "Hello world!" {
		t.Error("aCheck should not modify its input")
	}
	if got := aCheck(0, "Hello world?"); got != 20 {
		t.Errorf("aCheck(0, \"Hello world?\") = %d, want 20", got)
	}
	if got := aCheck(0, "Hello world. This is a test."); got != 30 {
		t.Errorf("aCheck(0, \"Hello world. This is a test.\") = %d, want 30", got)
	}
	if got := aCheck(0, "Hello world. this is a test."); got != 10 {
		t.Errorf("aCheck(0, \"Hello world. this is a test.\") = %d, want 10", got)
	}
	if got := aCheck(0, ""); got != 0 {
		t.Errorf("aCheck(0, \"\") = %d, want 0", got)
	}
}

func TestBCheck(t *testing.T) {
	body := "the quick brown fox"
	if got := bCheck("english", 0, body); got != 9 {
		t.Errorf("bCheck(\"english\", 0, %q) = %d, want 9", body, got)
	}
	body = "le rapide renard brun"
	if got := bCheck("french", 0, body); got != 6 {
		t.Errorf("bCheck(\"french\", 0, %q) = %d, want 6", body, got)
	}
}

func TestCCheck(t *testing.T) {
	body := "Hello world"
	if got := cCheck(0, body); got != 20 {
		t.Errorf("cCheck(0, %q) = %d, want 20", body, got)
	}
	body = "hello world"
	if got := cCheck(0, body); got != -10 {
		t.Errorf("cCheck(0, %q) = %d, want -10", body, got)
	}
}

func TestDCheck(t *testing.T) {
	body := "Hellooo world"
	if got := dCheck(0, body); got != -50 {
		t.Errorf("dCheck(0, %q) = %d, want -50", body, got)
	}
	body = "Hello world"
	if got := dCheck(0, body); got != 0 {
		t.Errorf("dCheck(0, %q) = %d, want 0", body, got)
	}
}

func TestECheck(t *testing.T) {
	body := "Hello world"
	if got := eCheck(0, body); got != 20 {
		t.Errorf("eCheck(0, %q) = %d, want 20", body, got)
	}
	body = "Helloworld"
	if got := eCheck(0, body); got != 20 {
		t.Errorf("eCheck(0, %q) = %d, want 20", body, got)
	}
	body = "     "
	if got := eCheck(0, body); got != -20 {
		t.Errorf("eCheck(0, %q) = %d, want -20", body, got)
	}
}

func TestFCheck(t *testing.T) {
	body := strings.Repeat("a", 76)
	if got := fCheck(0, body); got != -150 {
		t.Errorf("fCheck(0, 76×'a') = %d, want -150", got)
	}
	body = strings.Repeat("a", 75) + "."
	if got := fCheck(0, body); got != 0 {
		t.Errorf("fCheck(0, 75×'a'+'.') = %d, want 0", got)
	}
}

func TestGCheck(t *testing.T) {
	body := strings.Repeat("a", 31) + "  " + strings.Repeat("b", 31)
	if got := gCheck(0, body); got != 0 {
		t.Errorf("gCheck(0, 31×'a'+'  '+31×'b') = %d, want 0", got)
	}
	body = strings.Repeat("a", 64)
	if got := gCheck(0, body); got != -40 {
		t.Errorf("gCheck(0, 64×'a') = %d, want -40", got)
	}
}
