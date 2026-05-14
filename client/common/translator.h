#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <string>
#include <wchar.h>

#include "utils.h"

const wchar_t wwCharacterDictionary[] = {
    L'\0', L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O',
    L'P', L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'a', L'b', L'c', L'd', L'e',
    L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p', L'q', L'r', L's', L't', L'u',
    L'v', L'w', L'x', L'y', L'z', L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'⨍',
    L'ⓢ', L'Œ', L'Ž', L'š', L'œ', L'ž', L'Ÿ', L'À', L'Á', L'Â', L'Ã', L'Ä', L'Å', L'Æ', L'Ç', L'È',
    L'É', L'Ê', L'Ë', L'Ì', L'Í', L'Î', L'Ï', L'Đ', L'Ñ', L'Ò', L'Ó', L'Ô', L'Õ', L'Ö', L'Ø', L'Ù',
    L'Ú', L'Û', L'Ü', L'Ý', L'Þ', L'β', L'à', L'á', L'â', L'ã', L'ä', L'å', L'æ', L'ç', L'è', L'é',
    L'ê', L'ë', L'ì', L'í', L'î', L'ï', L'ð', L'ñ', L'ò', L'ó', L'ô', L'õ', L'ö', L'ø', L'ù', L'ú',
    L'û', L'ü', L'ý', L'þ', L'ÿ', L' ', L'\n', L'!', L'"', L'#', L'$', L'%', L'&', L'´', L'(', L')',
    L'*', L'+', L',', L'-', L'.', L'/', L':', L';', L'<', L'=', L'>', L'?', L'@', L'[', L'\\', L']',
    L'^', L'_', L'´', L'{', L'|', L'}', L'~', L'€', L'‚', L'„', L'…', L'†', L'‡', L'＾', L'‰', L'⟨',
    L'‘', L'’', L'“', L'”', L'•', L'–', L'—', L'˜', L'™', L'⟩', L' ', L'¡', L'¢', L'£', L'¤', L'¥',
    L'╎', L'§', L'¨', L'©', L'ª', L'«', L'¬', L'-', L'®', L'‾', L'°', L'±', L'²', L'³', L'⁄', L'µ',
    L'¶', L'•', L'¸', L'¹', L'º', L'»', L'¼', L'½', L'¾', L'¿', L'×', L'÷', L'☔', L'★', L'❤', L'♪',
    L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0',
    L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0'
};

const wchar_t wwCharacterDictionaryJapanese[] = {
    L'\0', L'あ', L'い', L'う', L'え', L'お', L'か', L'き', L'く', L'け', L'こ', L'さ', L'し', L'す', L'せ', L'そ',
    L'た', L'ち', L'つ', L'て', L'と', L'な', L'に', L'ぬ', L'ね', L'の', L'は', L'ひ', L'ふ', L'へ', L'ほ', L'ま',
    L'み', L'む', L'め', L'も', L'や', L'ゆ', L'よ', L'ら', L'り', L'る', L'れ', L'ろ', L'わ', L'を', L'ん', L'が',
    L'ぎ', L'ぐ', L'げ', L'ご', L'ざ', L'じ', L'ず', L'ぜ', L'ぞ', L'だ', L'ぢ', L'づ', L'で', L'ど', L'ば', L'び',
    L'ぶ', L'べ', L'ぼ', L'ぱ', L'ぴ', L'ぷ', L'ぺ', L'ぽ', L'ぁ', L'ぃ', L'ぅ', L'ぇ', L'ぉ', L'ゃ', L'ゅ', L'ょ',
    L'っ', L'ア', L'イ', L'ウ', L'エ', L'オ', L'カ', L'キ', L'ク', L'ケ', L'コ', L'サ', L'シ', L'ス', L'セ', L'ソ',
    L'タ', L'チ', L'ツ', L'テ', L'ト', L'ナ', L'二', L'ヌ', L'ネ', L'ノ', L'ハ', L'ヒ', L'フ', L'へ', L'ホ', L'マ',
    L'ミ', L'ム', L'メ', L'モ', L'ヤ', L'ユ', L'ヨ', L'ラ', L'リ', L'ル', L'レ', L'ロ', L'ワ', L'ヲ', L'ソ', L'ガ',
    L'ギ', L'グ', L'ゲ', L'ゴ', L'ザ', L'ジ', L'ズ', L'ゼ', L'ゾ', L'ダ', L'ヂ', L'ヅ', L'デ', L'ド', L'バ', L'ビ',
    L'ブ', L'ベ', L'ボ', L'パ', L'ピ', L'プ', L'ペ', L'ポ', L'ァ', L'ィ', L'ゥ', L'ェ', L'ォ', L'ャ', L'ュ', L'ョ',
    L'ッ', L'ヴ', L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N',
    L'O', L'P', L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'a', L'b', L'c', L'd',
    L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p', L'q', L'r', L's', L't',
    L'u', L'v', L'w', L'x', L'y', L'z', L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9',
    L' ', L'\n', L'ー', L'~', L'･', L'。', L'、', L'!', L'?', L'.', L',', L'｢', L'｣', L'(', L')', L'<',
    L'>', L'\'', L'\"', L'_', L'+', L'=', L'&', L'@', L':', L';', L'×', L'÷', L'☔', L'★', L'♥', L'♪'
};

static inline wchar_t decodeChar(unsigned char input, bool jpn) {
    return jpn ? wwCharacterDictionaryJapanese[input] : wwCharacterDictionary[input];
}

static inline unsigned char encodeChar(wchar_t input, bool jpn) {
    const wchar_t* table = wwCharacterDictionary;
    size_t size = sizeof(wwCharacterDictionary);
    if(jpn) {
        table = wwCharacterDictionaryJapanese;
        size = sizeof(wwCharacterDictionaryJapanese);
    }

    if(!jpn) { // special char replacement for USA_EUR, yup that's not cool but editing the table above is not an option so...
        if(input == 0x0027) return 0xB1; // replace ‘ for ‘
    }

    int index = -1;
    int length = size / sizeof(table[0]);
    int i = 0;
    while(index == -1 && i < length) { // we can probably optimize that by creating reversed array 
        if(table[i] == input) {
            index = i;
        }
        i += 1;
    }
    if(index == -1) {
        consolef("Unkown char %c %04x\n", input, input);
        return jpn ? 232 : 156; // ?
    }
    return index;
}


static inline std::string decode(const std::string& input, bool jpn, bool kor) {
    if(kor) {
        consolef("We do not support korea for now.\n");
        dsExit(1);
    }

    std::string output = std::string(input);
    const char* cinput = input.c_str();
    for(unsigned int i = 0; i < input.length(); i++) {
        output[i] = decodeChar(cinput[i], jpn);
    }
    return output;
}


static inline std::string encode(const std::string& input, bool jpn, bool kor) {
    if(kor) {
        consolef("We do not support korea for now.\n");
        dsExit(1);
    }

    std::string output = std::string(input);
    const char* cinput = output.c_str();
    for(unsigned int i = 0; i < input.length(); i++) {
        output[i] = encodeChar(cinput[i] & 0xFF, jpn);
    }
    return output;
}


#endif