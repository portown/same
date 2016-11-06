{-# LANGUAGE ForeignFunctionInterface #-}

module Same.Scene (
    ClickResult,
    crNoStatus,
    crEndGame,
    crTitleMenu,
    crBeginNormal,
    crBeginMask1,
    crBeginMask2,
    crBeginMask3,
    crBeginMask4,
    crClear,
    crAllClear,
    crReplay,
    crReplay0,
    crReplay1,
    crReplay2,
    crReplay3,
    crReplay4,
    crReplay5,
    crReplay6,
    crReplay7,
    crReplay8,
    crReplay9
) where

import Foreign.C.Types

#include "defines.h"


type ClickResult = CUChar

#{enum ClickResult,
 , crNoStatus = CR_NOSTATUS
 , crEndGame = CR_ENDGAME
 , crTitleMenu = CR_TITLEMENU
 , crBeginNormal = CR_BEGINNORMAL
 , crBeginMask1 = CR_BEGINMASK1
 , crBeginMask2 = CR_BEGINMASK2
 , crBeginMask3 = CR_BEGINMASK3
 , crBeginMask4 = CR_BEGINMASK4
 , crClear = CR_CLEAR
 , crAllClear = CR_ALLCLEAR
 , crReplay = CR_REPLAY
 , crReplay0 = CR_REPLAY0
 , crReplay1 = CR_REPLAY1
 , crReplay2 = CR_REPLAY2
 , crReplay3 = CR_REPLAY3
 , crReplay4 = CR_REPLAY4
 , crReplay5 = CR_REPLAY5
 , crReplay6 = CR_REPLAY6
 , crReplay7 = CR_REPLAY7
 , crReplay8 = CR_REPLAY8
 , crReplay9 = CR_REPLAY9
 }
