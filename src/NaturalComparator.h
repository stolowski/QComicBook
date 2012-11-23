/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include <QString>

class NaturalComparator
{
	private:
		const bool caseSenitive;
		bool cmpLetters(QChar l, QChar r) const;

	public:
		NaturalComparator(bool caseSens = false) : caseSenitive(caseSens)
		{}

		bool operator()(QString const& l, QString const& r) const;
};
