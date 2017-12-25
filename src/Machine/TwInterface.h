#pragma once
#include <AntTweakBar.h>

void MainTw();
void MoveStatsTw();
void MoveTw();
void MachineTw();

void TW_CALL ResetBtn(void *clientData);
void TW_CALL NewBlock(void *clientData);
void TW_CALL ChangeView(void *clientData);
void TW_CALL AddMove(void *clientData);
void TW_CALL ResetQueue(void *clientData);
void TW_CALL OpenCreator(void *clientData);

void TW_CALL getBlockMult(void *value, void *clientData);
void TW_CALL setBlockMult(const void *value, void *clientData);

void TW_CALL getBlockHMult(void *value, void *clientData);
void TW_CALL setBlockHMult(const void *value, void *clientData);

void TW_CALL getDrillR(void *value, void *clientData);
void TW_CALL setDrillR(const void *value, void *clientData);

void TW_CALL getSpeedRate(void *value, void *clientData);
void TW_CALL setSpeedRate(const void *value, void *clientData);

void TW_CALL getPosX(void *value, void *clientData);
void TW_CALL getPosY(void *value, void *clientData);
void TW_CALL getPosZ(void *value, void *clientData);

void TW_CALL getDesX(void *value, void *clientData);
void TW_CALL getDesY(void *value, void *clientData);
void TW_CALL getDesZ(void *value, void *clientData);

void TW_CALL getQueue(void *value, void *clientData);