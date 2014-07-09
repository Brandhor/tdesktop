/*
This file is part of Telegram Desktop,
an unofficial desktop messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014 John Preston, https://tdesktop.com
*/
#pragma once

void initEmoji();
EmojiPtr getEmoji(uint32 code);

void findEmoji(const QChar *ch, const QChar *e, const QChar *&newEmojiEnd, uint32 &emojiCode);

inline bool emojiEdge(const QChar *ch) {
	return true;

	switch (ch->unicode()) {
	case '.': case ',': case ':': case ';': case '!': case '?': case '#': case '@':
	case '(': case ')': case '[': case ']': case '{': case '}': case '<': case '>':
	case '+': case '=': case '-': case '_': case '*': case '/': case '\\': case '^': case '$':
	case '"': case '\'':
	case 8212: case 171: case 187: // --, <<, >>
		return true;
	}
	return false;
}

inline QString replaceEmojis(const QString &text) {
	QString result;
	const QChar *emojiEnd = text.unicode(), *e = text.cend();
	bool canFindEmoji = true, consumePrevious = false;
	QStringList protocols;
	protocols << "http://" << "https://" << "ftp://" << "itmss://";
	QString word;
	for (const QChar *ch = emojiEnd; ch != e;) {
		if (ch->isSpace())
			word = "";
		else {
			word.append(*ch);
		}

		QRegExp mailr("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
		mailr.setCaseSensitivity(Qt::CaseInsensitive);
		mailr.setPatternSyntax(QRegExp::RegExp);
		if (mailr.exactMatch(word))  {
			canFindEmoji = false;
		} else {
			foreach(const QString &protocol, protocols) {
				if (word.startsWith(protocol, Qt::CaseInsensitive)) {
					canFindEmoji = false;
					break;
				}
			}
		}

		uint32 emojiCode = 0;
		const QChar *newEmojiEnd = 0;
		if (canFindEmoji) {
			findEmoji(ch, e, newEmojiEnd, emojiCode);
		}
		if (emojiCode) {
//			if (newEmojiEnd < e && newEmojiEnd->unicode() == ' ') ++newEmojiEnd;
			if (result.isEmpty()) result.reserve(text.size());
			if (ch > emojiEnd + (consumePrevious ? 1 : 0)) {
				result.append(emojiEnd, ch - emojiEnd - (consumePrevious ? 1 : 0));
			}
			if (emojiCode > 65535) {
				result.append(QChar((emojiCode >> 16) & 0xFFFF));
			}
			result.append(QChar(emojiCode & 0xFFFF));

			ch = emojiEnd = newEmojiEnd;
			canFindEmoji = true;
			consumePrevious = false;
		} else {
			if (false && (ch->unicode() == QChar::Space || ch->unicode() == QChar::Nbsp)) {
				canFindEmoji = true;
				consumePrevious = true;
			} else if (emojiEdge(ch)) {
				canFindEmoji = true;
				consumePrevious = false;
			} else {
				canFindEmoji = false;
			}
			++ch;
		}
	}
	if (result.isEmpty()) return text;

	if (emojiEnd < e) result.append(emojiEnd, e - emojiEnd);
	return result;
}

EmojiPack emojiPack(DBIEmojiTab tab);
