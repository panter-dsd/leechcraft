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

#include "storage.h"
#include <stdexcept>
#include <QFile>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include <util/util.h>
#include <util/dblock.h>
#include "xmlsettingsmanager.h"
#include "account.h"

namespace LeechCraft
{
namespace Snails
{
	namespace
	{
		template<typename T>
		QByteArray Serialize (const T& t)
		{
			QByteArray result;
			QDataStream stream (&result, QIODevice::WriteOnly);
			stream << t;
			return result;
		}
	}

	Storage::Storage (QObject *parent)
	: QObject (parent)
	, Settings_ (QCoreApplication::organizationName (),
				QCoreApplication::applicationName () + "_Snails_Storage")
	{
		SDir_ = Util::CreateIfNotExists ("snails/storage");
	}

	namespace
	{
		QList<Message_ptr> MessageSaverProc (QList<Message_ptr> msgs, const QDir dir)
		{
			Q_FOREACH (Message_ptr msg, msgs)
			{
				if (msg->GetID ().isEmpty ())
					continue;

				const QString dirName = msg->GetID ().toHex ().right (3);

				QDir msgDir = dir;
				if (!dir.exists (dirName))
					msgDir.mkdir (dirName);
				if (!msgDir.cd (dirName))
				{
					qWarning () << Q_FUNC_INFO
							<< "unable to cd into"
							<< msgDir.filePath (dirName);
					continue;
				}

				QFile file (msgDir.filePath (msg->GetID ().toHex ()));
				file.open (QIODevice::WriteOnly);
				file.write (qCompress (msg->Serialize (), 9));
			}

			return msgs;
		}
	}

	void Storage::SaveMessages (Account *acc, const QList<Message_ptr>& msgs)
	{
		const QDir& dir = DirForAccount (acc);

		Q_FOREACH (Message_ptr msg, msgs)
			PendingSaveMessages_ [acc] [msg->GetID ()] = msg;

		auto watcher = new QFutureWatcher<QList<Message_ptr>> ();
		FutureWatcher2Account_ [watcher] = acc;

		auto future = QtConcurrent::run (MessageSaverProc, msgs, dir);
		watcher->setFuture (future);
		connect (watcher,
				SIGNAL (finished ()),
				this,
				SLOT (handleMessagesSaved ()));

		Q_FOREACH (Message_ptr msg, msgs)
		{
			if (msg->GetID ().isEmpty ())
				continue;

			AddMsgToFolders (msg, acc);
			UpdateCaches (msg);
		}
	}

	MessageSet Storage::LoadMessages (Account *acc)
	{
		MessageSet result;

		const QDir& dir = DirForAccount (acc);
		Q_FOREACH (auto str, dir.entryList (QDir::NoDotAndDotDot | QDir::Dirs))
		{
			QDir subdir = dir;
			if (!subdir.cd (str))
			{
				qWarning () << Q_FUNC_INFO
						<< "unable to cd to"
						<< str;
				continue;
			}

			Q_FOREACH (auto str, subdir.entryList (QDir::NoDotAndDotDot | QDir::Files))
			{
				QFile file (subdir.filePath (str));
				if (!file.open (QIODevice::ReadOnly))
				{
					qWarning () << Q_FUNC_INFO
							<< "unable to open"
							<< str
							<< file.errorString ();
					continue;
				}

				Message_ptr msg (new Message);
				try
				{
					msg->Deserialize (qUncompress (file.readAll ()));
				}
				catch (const std::exception& e)
				{
					qWarning () << Q_FUNC_INFO
							<< "error deserializing the message from"
							<< file.fileName ()
							<< e.what ();
					continue;
				}
				result << msg;
				UpdateCaches (msg);
			}
		}

		Q_FOREACH (auto msg, PendingSaveMessages_ [acc].values ())
		{
			result << msg;
			UpdateCaches (msg);
		}

		return result;
	}

	Message_ptr Storage::LoadMessage (Account *acc, const QByteArray& id)
	{
		if (PendingSaveMessages_ [acc].contains (id))
			return PendingSaveMessages_ [acc] [id];

		QDir dir = DirForAccount (acc);
		if (!dir.cd (id.toHex ().right (3)))
		{
			qWarning () << Q_FUNC_INFO
					<< "unable to cd to"
					<< dir.filePath (id.toHex ().right (3));
			throw std::runtime_error ("Unable to cd to the directory");
		}

		QFile file (dir.filePath (id.toHex ()));
		if (!file.open (QIODevice::ReadOnly))
		{
			qWarning () << Q_FUNC_INFO
					<< "unable to open"
					<< file.fileName ()
					<< file.errorString ();
			throw std::runtime_error ("Unable to open the message file");
		}

		Message_ptr msg (new Message);
		try
		{
			msg->Deserialize (qUncompress (file.readAll ()));
			UpdateCaches (msg);
		}
		catch (const std::exception& e)
		{
			qWarning () << Q_FUNC_INFO
					<< "error deserializing the message from"
					<< file.fileName ()
					<< e.what ();
			throw;
		}

		return msg;
	}

	QSet<QByteArray> Storage::LoadIDs (Account *acc)
	{
		QSet<QByteArray> result;

		const QDir& dir = DirForAccount (acc);
		Q_FOREACH (const auto& str,
				dir.entryList (QDir::NoDotAndDotDot | QDir::Dirs))
		{
			QDir subdir = dir;
			if (!subdir.cd (str))
			{
				qWarning () << Q_FUNC_INFO
						<< "unable to cd to"
						<< str;
				continue;
			}

			Q_FOREACH (const auto& str,
					subdir.entryList (QDir::NoDotAndDotDot | QDir::Files))
				result << QByteArray::fromHex (str.toUtf8 ());
		}

		result += PendingSaveMessages_ [acc].keys ().toSet ();

		return result;
	}

	QSet<QByteArray> Storage::LoadIDs (Account *acc, const QStringList& folder)
	{
		QSet<QByteArray> result;

		const QByteArray& ba = Serialize (folder.isEmpty () ? QStringList ("INBOX") : folder);

		QSqlQuery query (*BaseForAccount (acc));
		query.prepare ("SELECT msgId FROM folder2msg WHERE folder = :folder;");
		query.bindValue (":folder", ba);
		if (!query.exec ())
		{
			Util::DBLock::DumpError (query);
			throw std::runtime_error ("Query execution failed for fetching IDs.");
		}

		while (query.next ())
			result << query.value (0).toByteArray ();

		Q_FOREACH (auto msg, PendingSaveMessages_ [acc].values ())
			if (msg->GetFolders ().contains (folder))
				result << msg->GetID ();

		return result;
	}

	int Storage::GetNumMessages (Account *acc) const
	{
		int result = 0;

		const QDir& dir = DirForAccount (acc);
		Q_FOREACH (auto str, dir.entryList (QDir::NoDotAndDotDot | QDir::Dirs))
		{
			QDir subdir = dir;
			if (!subdir.cd (str))
			{
				qWarning () << Q_FUNC_INFO
						<< "unable to cd to"
						<< str;
				continue;
			}

			result += subdir.entryList (QDir::NoDotAndDotDot | QDir::Files).size ();
		}

		return result;
	}

	bool Storage::HasMessagesIn (Account *acc) const
	{
		return GetNumMessages (acc);
	}

	bool Storage::IsMessageRead (Account *acc, const QByteArray& id)
	{
		if (IsMessageRead_.contains (id))
			return IsMessageRead_ [id];

		return LoadMessage (acc, id)->IsRead ();
	}

	QDir Storage::DirForAccount (Account *acc) const
	{
		const QByteArray& id = acc->GetID ().toHex ();

		QDir dir = SDir_;
		if (!dir.exists (id))
			dir.mkdir (id);
		if (!dir.cd (id))
		{
			qWarning () << Q_FUNC_INFO
					<< "unable to cd into"
					<< dir.filePath (id);
			throw std::runtime_error ("Unable to cd to the dir");
		}

		return dir;
	}

	namespace
	{
		void InitStorageBase (QSqlDatabase_ptr base)
		{
			QHash<QString, QStringList> table2queries;
			table2queries ["folder2msg"] << "CREATE TABLE folder2msg "
					"(folder BLOB NOT NULL, msgId BLOB NOT NULL, UNIQUE (folder, msgId) ON CONFLICT IGNORE);";
			table2queries ["folder2msg"] << "CREATE INDEX folder2msg_idx_folder ON folder2msg (folder);";

			Q_FOREACH (const QString& key, table2queries.keys ())
				if (!base->tables ().contains (key))
					Q_FOREACH (const QString& queryStr, table2queries [key])
					{
						QSqlQuery query (*base);
						if (!query.exec (queryStr))
						{
							Util::DBLock::DumpError (query);
							throw std::runtime_error ("Query execution failed for storage creation.");
						}
					}

			QSqlQuery pragmas (*base);
			pragmas.exec ("PRAGMA synchronous = OFF;");
		}
	}

	QSqlDatabase_ptr Storage::BaseForAccount (Account *acc)
	{
		if (AccountBases_.contains (acc))
			return AccountBases_ [acc];

		const auto& dir = DirForAccount (acc);

		auto db = QSqlDatabase::addDatabase ("QSQLITE", "SnailsStorage_" + acc->GetID ());
		QSqlDatabase_ptr base (new QSqlDatabase (db));
		if (!base->isValid ())
		{
			qWarning () << Q_FUNC_INFO
					<< "database invalid :(";
			Util::DBLock::DumpError (base->lastError ());
			throw std::runtime_error ("Unable to add database connection.");
		}

		base->setDatabaseName (dir.filePath ("msgs.db"));
		if (!base->open ())
		{
			qWarning () << Q_FUNC_INFO;
			Util::DBLock::DumpError (base->lastError ());
			throw std::runtime_error (qPrintable (QString ("Could not initialize database: %1")
						.arg (base->lastError ().text ())));
		}

		InitStorageBase (base);
		AccountBases_ [acc] = base;
		return base;
	}

	void Storage::AddMsgToFolders (Message_ptr msg, Account *acc)
	{
		const auto& folders = msg->GetFolders ();
		const auto& id = msg->GetID ();

		auto base = BaseForAccount (acc);

		QSqlQuery query (*base);
		QStringList queries;
		queries << "INSERT INTO folder2msg (folder, msgId) VALUES (:folder, :msgId);";
		Q_FOREACH (const QString& qStr, queries)
		{
			query.prepare (qStr);
			Q_FOREACH (auto folder, folders)
			{
				if (folder.isEmpty ())
					folder << "INBOX";

				query.bindValue (":folder", Serialize (folder));
				query.bindValue (":msgId", id);
				if (!query.exec ())
					Util::DBLock::DumpError (query);
			}
			query.finish ();
		}
	}

	void Storage::UpdateCaches (Message_ptr msg)
	{
		IsMessageRead_ [msg->GetID ()] = msg->IsRead ();
	}

	void Storage::handleMessagesSaved ()
	{
		auto watcher = dynamic_cast<QFutureWatcher<QList<Message_ptr>>*> (sender ());
		watcher->deleteLater ();

		auto acc = FutureWatcher2Account_.take (watcher);
		if (!acc)
		{
			qWarning () << Q_FUNC_INFO
					<< "no account for future watcher"
					<< watcher;
			return;
		}

		auto& hash = PendingSaveMessages_ [acc];

		auto messages = watcher->result ();
		Q_FOREACH (Message_ptr msg, messages)
			hash.remove (msg->GetID ());
	}
}
}
