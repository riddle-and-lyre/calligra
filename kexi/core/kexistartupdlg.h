/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXISTARTUPDLG_H
#define KEXISTARTUPDLG_H

#include "kexistartupdlgui.h"

class KexiStartupDlg : public KexiStartupDlgUI
{
	Q_OBJECT

	public:
		enum Result
		{
			CreateNew = 0,
			CreateNewAlternative,
			OpenExisting,
			OpenRecent,
			Cancel
		};

		KexiStartupDlg(QWidget *parent);
		~KexiStartupDlg();

		const QString	fileName() { return m_file; }

	protected slots:
		void		checkRecent();
		void		checkNew();
		void		openRecentClicked();
		void		otherClicked();

		virtual	void	accept();
		virtual void	reject();

	private:
		QString		m_file;
};

#endif

