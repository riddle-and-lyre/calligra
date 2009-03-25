/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "test.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] = "A test application for the KoProperty library";

static const char version[] = "0.2";

int main(int argc, char **argv)
{
    KAboutData about("propertytest2", 0, ki18n("KoProperty Test"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2005 Cedric Pasteur"), KLocalizedString(), 0, "cedric.pasteur@free.fr");
    about.addAuthor(ki18n("Cedric Pasteur"), KLocalizedString(), "cedric.pasteur@free.fr");
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("flat", ki18n("Flat display: don't display groups\n(useful for testing)"));
    options.add("ro", ki18n("Set all properties as read-only:\n(useful for testing read-only mode)"));
    options.add("property <name>", ki18n("Display only specified property\n(useful when we want to focus on testing a single property editor)"));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    TestWindow test;
    test.show();

    return app.exec();
}

