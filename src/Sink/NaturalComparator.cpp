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
#include "NaturalComparator.h"

using namespace QComicBook;

static unsigned toInt(QString const& s, int *idx)
{
	int res = 0;
	int i = *idx;
	while (i != s.size() && s[i].isDigit())
	{
		res *= 10;
		res += s[i].digitValue();
		++i;
	}
	*idx = i;
	return res;
}

bool NaturalComparator::cmpLetters(QChar l, QChar r) const
{
	return this->caseSenitive ? l < r : l.toLower() < r.toLower();
}

// Adapted from Dave Koelle's Alphanum (http://www.davekoelle.com/alphanum.html)
// The idea is to split the string into "chunks" of letters or digits
bool NaturalComparator::operator()(QString const& l, QString const& r) const
{
	enum Mode { String, Number } mode = String;
	for (int il = 0, ir = 0; il != l.size() && ir != r.size();)
	{
		if (mode == String)
		{
			for (; il != l.size() && ir != r.size(); ++il, ++ir)
			{
				const bool digitL = l[il].isDigit(), digitR = r[ir].isDigit();
				if (digitL && digitR)
				{
					mode = Number;
					break;
				}
				if (digitL) return true;
				if (digitR) return false;
				if (l[il] != r[ir]) return l[il] < r[ir];
			}		
		}
		else
		{
			// At least the current character is a digit in both strings
		    unsigned lNum = toInt(l, &il);
			unsigned rNum = toInt(r, &ir);
			if (lNum != rNum) return lNum < rNum;
			mode = String;
		}
	}
	return l.size() < r.size();
}
