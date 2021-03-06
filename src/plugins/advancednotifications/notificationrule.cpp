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

#include "notificationrule.h"
#include <QStringList>
#include <QtDebug>

namespace LeechCraft
{
namespace AdvancedNotifications
{
	bool operator== (const VisualParams&, const VisualParams&)
	{
		return true;
	}

	AudioParams::AudioParams ()
	{
	}

	AudioParams::AudioParams (const QString& fname)
	: Filename_ (fname)
	{
	}

	bool operator== (const AudioParams& ap1, const AudioParams& ap2)
	{
		return ap1.Filename_ == ap2.Filename_;
	}

	bool operator== (const TrayParams&, const TrayParams&)
	{
		return true;
	}

	CmdParams::CmdParams ()
	{
	}

	CmdParams::CmdParams (const QString& cmd, const QStringList& args)
	: Cmd_ (cmd)
	, Args_ (args)
	{
	}

	bool operator== (const CmdParams& cp1, const CmdParams& cp2)
	{
		return cp1.Args_ == cp2.Args_ &&
			cp1.Cmd_ == cp2.Cmd_;
	}

	NotificationRule::NotificationRule ()
	: Methods_ (NMNone)
	, IsEnabled_ (true)
	, IsSingleShot_ (false)
	{
	}

	NotificationRule::NotificationRule (const QString& name,
			const QString& cat, const QStringList& types)
	: Name_ (name)
	, Category_ (cat)
	, Types_ (types)
	, Methods_ (NMNone)
	, IsEnabled_ (true)
	, IsSingleShot_ (false)
	{
	}

	bool NotificationRule::IsNull () const
	{
		return Name_.isEmpty () ||
				Category_.isEmpty () ||
				Types_.isEmpty ();
	}

	QString NotificationRule::GetName () const
	{
		return Name_;
	}

	void NotificationRule::SetName (const QString& name)
	{
		Name_ = name;
	}

	QString NotificationRule::GetCategory () const
	{
		return Category_;
	}

	void NotificationRule::SetCategory (const QString& cat)
	{
		Category_ = cat;
	}

	QStringList NotificationRule::GetTypes () const
	{
		return Types_;
	}

	void NotificationRule::SetTypes (const QStringList& types)
	{
		Types_ = types;
	}

	NotificationMethods NotificationRule::GetMethods () const
	{
		return Methods_;
	}

	void NotificationRule::SetMethods (const NotificationMethods& methods)
	{
		Methods_ = methods;
	}

	FieldMatches_t NotificationRule::GetFieldMatches () const
	{
		return FieldMatches_;
	}

	VisualParams NotificationRule::GetVisualParams () const
	{
		return VisualParams_;
	}

	void NotificationRule::SetVisualParams (const VisualParams& params)
	{
		VisualParams_ = params;
	}

	AudioParams NotificationRule::GetAudioParams () const
	{
		return AudioParams_;
	}

	void NotificationRule::SetAudioParams (const AudioParams& params)
	{
		AudioParams_ = params;
	}

	TrayParams NotificationRule::GetTrayParams () const
	{
		return TrayParams_;
	}

	void NotificationRule::SetTrayParams (const TrayParams& params)
	{
		TrayParams_ = params;
	}

	CmdParams NotificationRule::GetCmdParams() const
	{
		return CmdParams_;
	}

	void NotificationRule::SetCmdParams (const CmdParams& params)
	{
		CmdParams_ = params;
	}

	bool NotificationRule::IsEnabled () const
	{
		return IsEnabled_;
	}

	void NotificationRule::SetEnabled (bool enabled)
	{
		IsEnabled_ = enabled;
	}

	bool NotificationRule::IsSingleShot () const
	{
		return IsSingleShot_;
	}

	void NotificationRule::SetSingleShot (bool shot)
	{
		IsSingleShot_ = shot;
	}

	void NotificationRule::SetFieldMatches (const FieldMatches_t& matches)
	{
		FieldMatches_ = matches;
	}

	void NotificationRule::Save (QDataStream& stream) const
	{
		stream << static_cast<quint8> (3)
			<< Name_
			<< Category_
			<< Types_
			<< static_cast<quint16> (Methods_)
			<< AudioParams_.Filename_
			<< CmdParams_.Cmd_
			<< CmdParams_.Args_
			<< IsEnabled_
			<< IsSingleShot_
			<< static_cast<quint16> (FieldMatches_.size ());

		Q_FOREACH (const FieldMatch& match, FieldMatches_)
			match.Save (stream);
	}

	void NotificationRule::Load (QDataStream& stream)
	{
		quint8 version = 0;
		stream >> version;
		if (version < 1 || version > 3)
		{
			qWarning () << Q_FUNC_INFO
					<< "unknown version"
					<< version;
			return;
		}

		quint16 methods;
		stream >> Name_
			>> Category_
			>> Types_
			>> methods
			>> AudioParams_.Filename_;

		if (version >= 2)
			stream >> CmdParams_.Cmd_
				>> CmdParams_.Args_;

		if (version >= 3)
			stream >> IsEnabled_
				>> IsSingleShot_;
		else
		{
			IsEnabled_ = true;
			IsSingleShot_ = false;
		}

		Methods_ = static_cast<NotificationMethods> (methods);

		quint16 numMatches = 0;
		stream >> numMatches;

		for (int i = 0; i < numMatches; ++i)
		{
			FieldMatch match;
			match.Load (stream);
			FieldMatches_ << match;
		}
	}

	bool operator== (const NotificationRule& r1, const NotificationRule& r2)
	{
		return r1.GetMethods () == r2.GetMethods () &&
			r1.IsEnabled () == r2.IsEnabled () &&
			r1.IsSingleShot () == r2.IsSingleShot () &&
			r1.GetName () == r2.GetName () &&
			r1.GetCategory () == r2.GetCategory () &&
			r1.GetTypes () == r2.GetTypes () &&
			r1.GetFieldMatches () == r2.GetFieldMatches () &&
			r1.GetVisualParams () == r2.GetVisualParams () &&
			r1.GetAudioParams () == r2.GetAudioParams () &&
			r1.GetTrayParams () == r2.GetTrayParams () &&
			r1.GetCmdParams () == r2.GetCmdParams ();
	}
}
}
