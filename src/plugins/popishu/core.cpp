/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2010  Georg Rudoy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include "core.h"
#include "editorpage.h"

namespace LeechCraft
{
	namespace Plugins
	{
		namespace Popishu
		{
			Core::Core ()
			{
			}

			Core& Core::Instance ()
			{
				static Core c;
				return c;
			}

			EditorPage* Core::NewTabRequested ()
			{
				EditorPage *page = MakeEditorPage ();
				emit addNewTab ("Popishu", page);
				emit raiseTab (page);

				return page;
			}

			void Core::Handle (const Entity& e)
			{
				EditorPage *page = NewTabRequested ();
				page->SetText (e.Entity_.toString ());

				QString language = e.Additional_ ["Language"].toString ();
				if (!language.isEmpty ())
					page->SetLanguage (language);
			}

			EditorPage* Core::MakeEditorPage ()
			{
				EditorPage *result = new EditorPage ();
				connect (result,
						SIGNAL (removeTab (QWidget*)),
						this,
						SIGNAL (removeTab (QWidget*)));
				connect (result,
						SIGNAL (changeTabName (QWidget*, const QString&)),
						this,
						SIGNAL (changeTabName (QWidget*, const QString&)));
				return result;
			}
		};
	};
};
