/***************************************************************************
 *   Copyright (C) 2016 by Jean-Baptiste Mardelle (jb@kdenlive.org)        *
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

#ifndef PREVIEWMANAGER_H
#define PREVIEWMANAGER_H

#include "definitions.h"

#include <QDir>
#include <QMutex>
#include <QTimer>
#include <QFuture>

class KdenliveDoc;
class CustomRuler;

namespace Mlt {
    class Tractor;
    class Playlist;
}

/**
 * @namespace PreviewManager
 * @brief Handles timeline preview.
 */

class PreviewManager : public QObject
{
    Q_OBJECT

public:
    explicit PreviewManager(KdenliveDoc *doc, CustomRuler *ruler, Mlt::Tractor *tractor);
    virtual ~PreviewManager();
    /** @brief: initialize base variables, return false if error. */
    bool initialize();
    void invalidatePreviews(QList <int> chunks);
    void addPreviewRange(bool add);
    void abortRendering();
    bool loadParams();
    void invalidatePreview(int startFrame, int endFrame);
    bool buildPreviewTrack();
    void reconnectTrack();
    void disconnectTrack();

private:
    KdenliveDoc *m_doc;
    CustomRuler *m_ruler;
    Mlt::Tractor *m_tractor;
    Mlt::Playlist *m_previewTrack;
    QDir m_cacheDir;
    QDir m_undoDir;
    QMutex m_previewMutex;
    QStringList m_consumerParams;
    QString m_extension;
    QTimer m_previewTimer;
    QTimer m_previewGatherTimer;
    bool m_initialized;
    bool m_abortPreview;
    QList <int> m_waitingThumbs;
    QFuture <void> m_previewThread;

private slots:
    void doCleanupOldPreviews();
    void doPreviewRender(QString scene);
    void slotRemoveInvalidUndo(int ix);
    void slotReloadChunks(QDir cacheDir, QList <int> chunks, const QString ext);
    void slotProcessDirtyChunks();

public slots:
    void startPreviewRender();
    void gotPreviewRender(int frame, const QString &file, int progress);

signals:
    void abortPreview();
    void cleanupOldPreviews();
    void previewRender(int frame, const QString &file, int progress);
};

#endif
