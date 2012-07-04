/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2012  Georg Rudoy
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

#pragma once

#include <QObject>
#include <interfaces/core/icoreproxy.h>

namespace LeechCraft
{
struct Entity;

namespace LMP
{
	class LocalCollection;
	class LocalFileResolver;
	class PlaylistManager;

	class Core : public QObject
	{
		Q_OBJECT

		ICoreProxy_ptr Proxy_;

		LocalFileResolver *Resolver_;
		LocalCollection *Collection_;
		PlaylistManager *PLManager_;

		QObjectList SyncPlugins_;

		Core ();
	public:
		static Core& Instance ();

		void SetProxy (ICoreProxy_ptr);
		ICoreProxy_ptr GetProxy ();

		void SendEntity (const Entity&);

		void PostInit ();

		void AddPlugin (QObject*);
		QList<QObject*> GetSyncPlugins () const;

		LocalFileResolver* GetLocalFileResolver () const;
		LocalCollection* GetLocalCollection () const;
		PlaylistManager* GetPlaylistManager () const;
	public slots:
		void rescan ();
	signals:
		void gotEntity (const LeechCraft::Entity&);
	};
}
}
