/***************************************************************************
 *   Copyright (C) 2010 by Pascal Fleury (fleury@users.sourceforge.net)    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#include "jogaction.h"
#include "core.h"
#include "monitor/monitormanager.h"

#include <stdio.h>
#include <stdlib.h>
#include <klocalizedstring.h>
#include <QDebug>

// TODO(fleury): this should probably be a user configuration parameter (at least the max speed).
//const double SPEEDS[] = {0.0, 0.5, 1.0, 2.0, 4.0, 8.0, 16.0, 32.0};
const double SPEEDS[] = {0.0, 1.0, 2.0, 4.0, 5.0, 8.0, 16.0, 60.0};
const size_t SPEEDS_SIZE = sizeof(SPEEDS) / sizeof(double);

JogShuttleAction::JogShuttleAction (const JogShuttle* jogShuttle, const QStringList& actionMap, QObject * parent)
        : QObject(parent), m_jogShuttle(jogShuttle), m_actionMap(actionMap)
{
    // Add action map 0 used for stopping the monitor when the shuttle is in neutral position.
    if (m_actionMap.size() == 0)
      m_actionMap.append(QStringLiteral("monitor_pause"));

    connect(m_jogShuttle, SIGNAL(jogBack()), pCore->monitorManager(), SLOT(slotRewindOneFrame()));
    connect(m_jogShuttle, SIGNAL(jogForward()), pCore->monitorManager(), SLOT(slotForwardOneFrame()));
    connect(m_jogShuttle, SIGNAL(shuttlePos(int)), SLOT(slotShuttlePos(int)));
    connect(m_jogShuttle, SIGNAL(button(int)), SLOT(slotButton(int)));

    connect(this, SIGNAL(rewind(double)), pCore->monitorManager(), SLOT(slotRewind(double)));
    connect(this, SIGNAL(forward(double)), pCore->monitorManager(), SLOT(slotForward(double)));
}

JogShuttleAction::~JogShuttleAction()
{
}

void JogShuttleAction::slotShuttlePos(int shuttle_pos)
{
    size_t magnitude = abs(shuttle_pos);
    if (magnitude < SPEEDS_SIZE) {
        if (shuttle_pos < 0)
            emit rewind(-SPEEDS[magnitude]);
        if (shuttle_pos == 0) {
            ////qDebug() << "Shuttle pos0 action: " << m_actionMap[0];
            emit action(m_actionMap[0]);
        }
        if (shuttle_pos > 0)
            emit forward(SPEEDS[magnitude]);
    }
}

void JogShuttleAction::slotButton(int button_id)
{
    if (button_id >= m_actionMap.size() || m_actionMap[button_id].isEmpty()) {
        // TODO(fleury): Shoudl this go to the status bar to inform the user ?
        //qDebug() << "No action applied for button: " << button_id;
        return;
    }
    ////qDebug() << "Shuttle button =" << button_id << ": action=" << m_actionMap[button_id];
    emit action(m_actionMap[button_id]);
}


