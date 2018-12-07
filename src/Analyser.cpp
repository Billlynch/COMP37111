//
// Created by Bill Lynch on 15/10/2018.
//

#include "Analyser.h"

void Analyser::addData(unsigned long frameCount, double frameDelta, double physicsDelta, double openGLPushDelta,
                       double drawCallDelta, bool *spaceHeld) {
    outputFile << frameCount << "," << frameDelta << "," << physicsDelta << "," << openGLPushDelta << ","
               << drawCallDelta << "," << (*spaceHeld ? "true" : "false")
               << std::endl;
}
