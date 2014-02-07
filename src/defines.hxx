// defines.h

#pragma once


// ==============================================
// �萔��`�w�b�_�[
// ==============================================

#define MINE_TIMER 1000

#define DATA( fname ) "data\\" #fname
#define _DELETE( x ); if ( x ) { delete x; x = NULL; }
#define CODE( x ) ( ( ( x ) * 7 % 10 * 10 ) & 0xf0 ) |\
  ( ( ( ( char )sizeof( unsigned long ) - ( x ) ) * 77 % 100 ) & 0x0f )

#define ADDSCORE( x ) ( unsigned long )pow( 1.6, x )
#define PIX 32    // ��̕�
#define PIY 32    // ��̍���
#define MASKMAX 4 // �}�X�N���[�h���E

#define SELWAV DATA( select.wav )
#define ERSWAV DATA( erase.wav )
#define CLRWAV DATA( clear.wav )
#define DATFILE ".\\same.dat"
#define REPFILE ".\\replay"

// �N���b�N�߂�l
enum CLICKRESULT
{
  CR_NOSTATUS = 0,
  CR_ENDGAME,
  CR_TITLEMENU,
  CR_BEGINNORMAL,
  CR_BEGINMASK1,
  CR_BEGINMASK2,
  CR_BEGINMASK3,
  CR_BEGINMASK4,
  CR_CLEAR,
  CR_ALLCLEAR,
  CR_REPLAY,
  CR_REPLAY0,
  CR_REPLAY1,
  CR_REPLAY2,
  CR_REPLAY3,
  CR_REPLAY4,
  CR_REPLAY5,
  CR_REPLAY6,
  CR_REPLAY7,
  CR_REPLAY8,
  CR_REPLAY9,
  CR_CRITICALERROR = 0xff
};

// �Q�[���̏��
enum GAMESTATUS
{
  GS_NOSTATUS = 0,
  GS_TITLEMENU,
  GS_PLAYING,
  GS_CLEAR,
  GS_ALLCLEAR,
  GS_NOREPLAY,
  GS_CRITICALERROR = 0xff
};


// EOF
