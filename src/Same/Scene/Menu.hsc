{-# LANGUAGE ForeignFunctionInterface #-}

module Same.Scene.Menu (
    GameSceneMenu,
    createGameSceneMenu,
    destroyGameSceneMenu,
    gameSceneMenuDraw,
    gameSceneMenuMouseMove,
    gameSceneMenuMouseLDown,
    gameSceneMenuKeyDown
) where

import Control.Applicative ((<$>), (<*>))
import Control.Exception (tryJust)
import Control.Monad (guard, when)
import Data.Bits ((.|.))
import qualified Data.ByteString as BS
import Data.Foldable (for_)
import Data.Int (Int8)
import Data.Maybe (maybe)
import Data.Word (Word8, Word16)
import qualified Graphics.Win32 as Win32
import Foreign.C.String
import Foreign.C.Types
import Foreign.Marshal.Alloc (mallocBytes, free)
import Foreign.Marshal.Utils (new)
import Foreign.Ptr (Ptr, nullPtr)
import Foreign.Storable
import System.IO
import qualified System.IO.Error as E

import Same.Graphics.Surface
import Same.Scene


#include "menu.h"


data GameSceneMenu = GameSceneMenu
                   { surface :: SurfacePtr
                   , level :: Word8
                   , width :: Word16
                   , height :: Word16
                   , selectedPosition :: Word8
                   , maskLevel :: Int8
                   , replayNumber :: Int8
                   , menuArea :: Win32.LPRECT
                   , leftArrowArea :: Win32.LPRECT
                   , rightArrowArea :: Win32.LPRECT
                   , menuSurface :: SurfacePtr
                   } deriving Show

instance Storable GameSceneMenu where
        sizeOf _ = #{size GameSceneMenu}
        alignment = sizeOf

        peek ptr = GameSceneMenu <$> (#{peek GameSceneMenu, surface} ptr)
                                 <*> (toWord8 <$> #{peek GameSceneMenu, m_Level} ptr)
                                 <*> (toWord16 <$> #{peek GameSceneMenu, m_Width} ptr)
                                 <*> (toWord16 <$> #{peek GameSceneMenu, m_Height} ptr)
                                 <*> (toWord8 <$> #{peek GameSceneMenu, m_Sel} ptr)
                                 <*> (toInt8 <$> #{peek GameSceneMenu, m_MaskNum} ptr)
                                 <*> (toInt8 <$> #{peek GameSceneMenu, m_RepNum} ptr)
                                 <*> (#{peek GameSceneMenu, m_rcMenu} ptr)
                                 <*> (#{peek GameSceneMenu, m_rcLeft} ptr)
                                 <*> (#{peek GameSceneMenu, m_rcRight} ptr)
                                 <*> (#{peek GameSceneMenu, menuSurface} ptr)
            where
                toWord8 = fromIntegral :: CUChar -> Word8
                toWord16 = fromIntegral :: CUShort -> Word16
                toInt8 = fromIntegral :: CChar -> Int8

        poke ptr s = do
                #{poke GameSceneMenu, surface} ptr $ surface s
                #{poke GameSceneMenu, m_Level} ptr $ toCUChar $ level s
                #{poke GameSceneMenu, m_Width} ptr $ toCUShort $ width s
                #{poke GameSceneMenu, m_Height} ptr $ toCUShort $ height s
                #{poke GameSceneMenu, m_Sel} ptr $ toCUChar $ selectedPosition s
                #{poke GameSceneMenu, m_MaskNum} ptr $ toCChar $ maskLevel s
                #{poke GameSceneMenu, m_RepNum} ptr $ toCChar $ replayNumber s
                #{poke GameSceneMenu, m_rcMenu} ptr $ menuArea s
                #{poke GameSceneMenu, m_rcLeft} ptr $ leftArrowArea s
                #{poke GameSceneMenu, m_rcRight} ptr $ rightArrowArea s
                #{poke GameSceneMenu, menuSurface} ptr $ menuSurface s
            where
                toCUChar = fromIntegral :: Word8 -> CUChar
                toCUShort = fromIntegral :: Word16 -> CUShort
                toCChar = fromIntegral :: Int8 -> CChar

type GameSceneMenuPtr = Ptr GameSceneMenu


createGameSceneMenu :: CUShort -> CUShort -> IO GameSceneMenuPtr
createGameSceneMenu w h = do
        lv <- loadLevel
        let menuTop = 260
        let menuRight = 230 + 180
        let menuBottom = 260 + 160
        menuRect <- newRect 230 menuTop menuRight menuBottom
        leftArrowRect <- newRect menuRight menuTop (menuRight + 20) menuBottom
        rightArrowRect <- newRect (menuRight + 80) menuTop (menuRight + 100) menuBottom
        surf <- withCString "data/title.bmp" surfaceFromBitmapFile
        menuSurf <- withCString "data/menu.bmp" surfaceFromBitmapFile
        new $ GameSceneMenu surf lv (fromIntegral w) (fromIntegral h) 0 0 (-1) menuRect leftArrowRect rightArrowRect menuSurf
    where
        newRect :: Win32.LONG -> Win32.LONG -> Win32.LONG -> Win32.LONG -> IO Win32.LPRECT
        newRect l t r b = do
            p <- mallocBytes 16
            Win32.pokeRECT p (l, t, r, b)
            return p

destroyGameSceneMenu :: GameSceneMenuPtr -> IO ()
destroyGameSceneMenu ptr = do
        s <- peek ptr
        saveLevel $ level s
        destroySurface $ menuSurface s
        destroySurface $ surface s
        free $ rightArrowArea s
        free $ leftArrowArea s
        free $ menuArea s
        free ptr

gameSceneMenuDraw :: GameSceneMenuPtr -> SurfacePtr -> IO ()
gameSceneMenuDraw scenePtr backSurface = do
        scene <- peek scenePtr
        surfaceBlit backSurface 0 0 (fromIntegral $ width scene) (fromIntegral $ height scene) (surface scene) 0 0 Win32.sRCCOPY

        let menu = menuSurface scene
        (menuLeft, menuTop, menuRight, menuBottom) <- fromIntegralRECT <$> (Win32.peekRECT $ menuArea scene)
        let (menuWidth, menuHeight) = (menuRight - menuLeft, menuBottom - menuTop)
        let itemHeight = menuHeight `div` 4

        let sel = selectedPosition scene

        for_ [i | i <- [0..3], level scene >= 1 || i /= 1] $ \i -> do
            let itemSrcX = if i + 1 == fromIntegral sel then menuWidth else 0
            let itemY = i * itemHeight
            surfaceBlit backSurface menuLeft (menuTop + itemY) menuWidth itemHeight menu itemSrcX itemY Win32.sRCCOPY

        (laLeft, _, laRight, _) <- fromIntegralRECT <$> (Win32.peekRECT $ leftArrowArea scene)
        (raLeft, _, _, _) <- fromIntegralRECT <$> (Win32.peekRECT $ rightArrowArea scene)

        when (level scene >= 1) $ do
            let y = menuTop + itemHeight

            let maskLaSrcX = if sel == 6 then 40 else 0
            surfaceBlit backSurface laLeft y 20 40 menu maskLaSrcX 160 Win32.sRCCOPY

            let maskRaSrcX = if sel == 10 then 60 else 20
            surfaceBlit backSurface raLeft y 20 40 menu maskRaSrcX 160 Win32.sRCCOPY

            let mask = fromIntegral $ maskLevel scene
            surfaceBlit backSurface (laRight + 36) y 19 40 menu (80 + mask * 19) 160 Win32.sRCCOPY

        let y = menuTop + 2 * itemHeight

        let replayLaSrcX = if sel == 7 then 40 else 0
        surfaceBlit backSurface laLeft y 20 40 menu replayLaSrcX 160 Win32.sRCCOPY

        let replayRaSrcX = if sel == 11 then 60 else 20
        surfaceBlit backSurface raLeft y 20 40 menu replayRaSrcX 160 Win32.sRCCOPY

        let rep = fromIntegral $ replayNumber scene
        if rep == (-1)
            then surfaceBlit backSurface (laRight + 5) y 50 40 menu 271 160 Win32.sRCCOPY
            else surfaceBlit backSurface (laRight + 36) y 19 40 menu (80 + rep * 19) 160 Win32.sRCCOPY

    where
        fromIntegralRECT :: Win32.RECT -> (CInt, CInt, CInt, CInt)
        fromIntegralRECT (l, t, r, b) = (fromIntegral l, fromIntegral t, fromIntegral r, fromIntegral b)

foreign import ccall "windows.h PlaySoundW" cplaySound :: Win32.LPCTSTR -> Win32.HMODULE -> Win32.DWORD -> IO Win32.BOOL
playSound :: String -> Win32.HMODULE -> Win32.DWORD -> IO ()
playSound name hModule flag = Win32.withTString name $ \t ->
    Win32.failIfFalse_ "PlaySound" $ cplaySound t hModule flag

#include <windows.h>

#{enum Win32.DWORD,
 , sND_ASYNC = SND_ASYNC
 , sND_FILENAME = SND_FILENAME
 , sND_SYNC = SND_SYNC
 }

gameSceneMenuMouseMove :: GameSceneMenuPtr -> Ptr Win32.POINT -> IO ()
gameSceneMenuMouseMove scenePtr pointPtr = do
        scene <- peek scenePtr
        let oldSelectedPosition = selectedPosition scene
        newSelectedPosition <- Win32.peekPOINT pointPtr >>= selectedPositionAt scene

        poke scenePtr $ scene { selectedPosition = newSelectedPosition }

        when (newSelectedPosition /= oldSelectedPosition && newSelectedPosition /= 0) $
            playSound "data\\select.wav" nullPtr (sND_FILENAME .|. sND_ASYNC)

    where
        selectedPositionAt :: GameSceneMenu -> Win32.POINT -> IO Word8
        selectedPositionAt scene pt = do
            menuRect <- Win32.peekRECT $ menuArea scene
            laRect <- Win32.peekRECT $ leftArrowArea scene
            raRect <- Win32.peekRECT $ rightArrowArea scene
            if ptInRect menuRect pt
                then do
                    let (_, menuTop, _, _) = menuRect
                    let pos = fromIntegral $ (snd pt - menuTop) `div` 40 + 1
                    return $ if pos == 2 then 0 else pos
                else if ptInRect laRect pt
                then do
                    let (_, laTop, _, _) = laRect
                    let pos = fromIntegral $ (snd pt - laTop) `div` 40 + 5
                    return $ if pos /= 7 && (level scene < 1 || pos /= 6) then 0 else pos
                else if ptInRect raRect pt
                then do
                    let (_, raTop, _, _) = raRect
                    let pos = fromIntegral $ (snd pt - raTop) `div` 40 + 9
                    return $ if pos /= 11 && (level scene < 1 || pos /= 10) then 0 else pos
                else return 0

        ptInRect :: Win32.RECT -> Win32.POINT -> Bool
        ptInRect (l, t, r, b) (x, y) = (x >= l && x < r && y >= t && y < b)

gameSceneMenuMouseLDown :: GameSceneMenuPtr -> IO CUChar
gameSceneMenuMouseLDown scenePtr = do
        scene <- peek scenePtr
        case selectedPosition scene of
            1 -> do
                playSound "data\\clear.wav" nullPtr (sND_FILENAME .|. sND_ASYNC)
                return $ crBeginNormal + (fromIntegral $ maskLevel scene)
            3 -> do
                playSound "data\\clear.wav" nullPtr (sND_FILENAME .|. sND_ASYNC)
                return $ crReplay0 + (fromIntegral $ replayNumber scene)
            4 -> do
                playSound "data\\clear.wav" nullPtr (sND_FILENAME .|. sND_SYNC)
                return crEndGame
            6 -> do
                playSound "data\\erase.wav" nullPtr (sND_FILENAME .|. sND_ASYNC)
                let newMaskLevel = maximum [maskLevel scene - 1, 0]
                poke scenePtr $ scene { maskLevel = newMaskLevel }
                return crNoStatus
            7 -> do
                playSound "data\\erase.wav" nullPtr (sND_FILENAME .|. sND_ASYNC)
                let newReplayNumber = maximum [replayNumber scene - 1, -1]
                poke scenePtr $ scene { replayNumber = newReplayNumber }
                return crNoStatus
            10 -> do
                playSound "data\\erase.wav" nullPtr (sND_FILENAME .|. sND_ASYNC)
                let newMaskLevel = minimum [maskLevel scene + 1, fromIntegral $ level scene, 4]
                poke scenePtr $ scene { maskLevel = newMaskLevel }
                return crNoStatus
            11 -> do
                playSound "data\\erase.wav" nullPtr (sND_FILENAME .|. sND_ASYNC)
                let newReplayNumber = minimum [replayNumber scene + 1, 9]
                poke scenePtr $ scene { replayNumber = newReplayNumber }
                return crNoStatus
            _ -> return crNoStatus

gameSceneMenuKeyDown :: GameSceneMenuPtr -> Win32.WPARAM -> IO ClickResult
gameSceneMenuKeyDown _ key
    | key == Win32.vK_ESCAPE = return crEndGame
    | otherwise = return crNoStatus


loadLevel :: IO Word8
loadLevel = do
        r <- tryJust (guard . E.isDoesNotExistError) $ do
            dat <- withBinaryFile "same.dat" ReadMode $ \hFile -> BS.hGetSome hFile 1
            return $ maybe 0 (fromIntegral . fst) $ BS.uncons dat
        case r of
            Right n -> return n
            _ -> return 0

saveLevel :: Word8 -> IO ()
saveLevel lv = do
        dat <- tryJust (guard . E.isDoesNotExistError) $ BS.uncons <$> BS.readFile "same.dat"
        case dat of
            Right (Just (_, rest)) -> BS.writeFile "same.dat" $ BS.cons (fromIntegral lv) rest
            _ -> BS.writeFile "same.dat" $ BS.singleton $ fromIntegral lv


foreign export ccall createGameSceneMenu :: CUShort -> CUShort -> IO GameSceneMenuPtr
foreign export ccall destroyGameSceneMenu :: GameSceneMenuPtr -> IO ()
foreign export ccall gameSceneMenuDraw :: GameSceneMenuPtr -> SurfacePtr -> IO ()
foreign export ccall gameSceneMenuMouseMove :: GameSceneMenuPtr -> Ptr Win32.POINT -> IO ()
foreign export ccall gameSceneMenuMouseLDown :: GameSceneMenuPtr -> IO ClickResult
foreign export ccall gameSceneMenuKeyDown :: GameSceneMenuPtr -> Win32.WPARAM -> IO ClickResult
