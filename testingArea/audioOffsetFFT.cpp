/*
Copyright (C) 2012  Simon A. Eugster (Granjow)  <simon.eu@gmail.com>
This file is part of kdenlive. See www.kdenlive.org.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
*/

#include "../src/lib/audio/audioEnvelope.h"
#include "../src/lib/audio/fftCorrelation.h"

#include <QCoreApplication>
#include <QStringList>
#include <QTime>
#include <QImage>
#include <QDebug>
#include <iostream>
#include <cmath>

void printUsage(const char *path)
{
    std::cout << "This executable takes two audio/video files A and B and determines " << std::endl
              << "how much B needs to be shifted in order to be synchronized with A." << std::endl
              << "Other than audioOffset this executable will use Fast Fourier Tranform " << std::endl
              << "which should be faster especially for large files." << std::endl << std::endl
              << path << " <main audio file> <second audio file>" << std::endl
              << "\t-h, --help\n\t\tDisplay this help" << std::endl
              << "\t--profile=<profile>\n\t\tUse the given profile for calculation (run: melt -query profiles)" << std::endl
              << "\t--no-images\n\t\tDo not save envelope and correlation images" << std::endl
                 ;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    args.removeAt(0);

    std::string profile = "atsc_1080p_24";
    bool saveImages = true;

    // Load arguments
    foreach (QString str, args) {

        if (str.startsWith("--profile=")) {
            QString s = str;
            s.remove(0, QString("--profile=").length());
            profile = s.toStdString();
            args.removeOne(str);

        } else if (str == "-h" || str == "--help") {
            printUsage(argv[0]);
            return 0;

        } else if (str == "--no-images") {
            saveImages = false;
            args.removeOne(str);
        }

    }

    if (args.length() < 2) {
        printUsage(argv[0]);
        return 1;
    }



    std::string fileMain(args.at(0).toStdString());
    args.removeFirst();
    std::string fileSub = args.at(0).toStdString();
    args.removeFirst();


    qDebug() << "Unused arguments: " << args;


    if (argc > 2) {
        fileMain = argv[1];
        fileSub = argv[2];
    } else {
        std::cout << "Usage: " << argv[0] << " <main audio file> <second audio file>" << std::endl;
        return 0;
    }
    std::cout << "Trying to align (2)\n\t" << fileSub << "\nto fit on (1)\n\t" << fileMain
              << "\n, result will indicate by how much (2) has to be moved." << std::endl
              << "Profile used: " << profile << std::endl
                 ;


    // Initialize MLT
    Mlt::Factory::init(NULL);

    // Load an arbitrary profile
    Mlt::Profile prof(profile.c_str());

    // Load the MLT producers
    Mlt::Producer prodMain(prof, fileMain.c_str());
    if (!prodMain.is_valid()) {
        std::cout << fileMain << " is invalid." << std::endl;
        return 2;
    }
    Mlt::Producer prodSub(prof, fileSub.c_str());
    if (!prodSub.is_valid()) {
        std::cout << fileSub << " is invalid." << std::endl;
        return 2;
    }


    // Build the audio envelopes for the correlation
    AudioEnvelope *envelopeMain = new AudioEnvelope(&prodMain);
    envelopeMain->loadEnvelope();
    envelopeMain->loadStdDev();
    envelopeMain->dumpInfo();

    AudioEnvelope *envelopeSub = new AudioEnvelope(&prodSub);
    envelopeSub->loadEnvelope();
    envelopeSub->loadStdDev();
    envelopeSub->dumpInfo();


    float *correlated;
    int corrSize = 0;

    FFTCorrelation::correlate(envelopeMain->envelope(), envelopeMain->envelopeSize(),
                              envelopeSub->envelope(), envelopeSub->envelopeSize(),
                              &correlated, corrSize);


    int maxIndex = 0;
    float max = 0;
    for (int i = 0; i < corrSize; i++) {
        if (correlated[i] > max) {
            max = correlated[i];
            maxIndex = i;
        }
    }
    int shift = envelopeMain->envelopeSize() - maxIndex-1;
    qDebug() << "Max correlation value is " << max << " at " << maxIndex;
    qDebug() << "Will have to move by " << shift << " frames";

    std::cout << fileSub << " should be shifted by " << shift << " frames" << std::endl
              << "\trelative to " << fileMain << std::endl
              << "\tin a " << prodMain.get_fps() << " fps profile (" << profile << ")." << std::endl
                 ;

    if (saveImages) {
        QString filename = QString("correlation-fft-%1.png")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh:mm:ss"));
        QImage img(corrSize/2, 400, QImage::Format_ARGB32);
        img.fill(qRgb(255,255,255));
        for (int x = 0; x < img.width(); x++) {
            float val = fabs(correlated[x]/max);
            for (int y = 0; y < img.height()*val; y++) {
                img.setPixel(x, img.height()-1-y, qRgb(50,50,50));
            }
        }
        img.save(filename);
        qDebug() << "Saved image to " << filename;
    }


    delete correlated;

}