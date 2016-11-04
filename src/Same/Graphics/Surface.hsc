{-# LANGUAGE ForeignFunctionInterface #-}

module Same.Graphics.Surface (
    Surface(..),
    SurfacePtr,
    createSurface,
    destroySurface,
    surfaceFromBitmapFile,
    surfaceFromResource,
    surfacePaintRect,
    surfaceDrawText,
    surfaceBlit
) where

import Control.Applicative ((<$>), (<*>))
import Control.Exception (bracket, finally)
import Control.Monad (mfilter)
import Data.Bits ((.|.))
import Data.Foldable (sequenceA_)
import Foreign.C.String
import Foreign.C.Types
import Foreign.Marshal.Alloc (free)
import Foreign.Marshal.Utils
import Foreign.Ptr (Ptr, nullPtr)
import Foreign.Storable
import qualified Graphics.Win32 as Win32

#include "surface.h"

data Surface = Surface
             { surfaceDC :: Win32.HDC
             , surfaceBitmap :: Win32.HBITMAP
             } deriving Show

instance Storable Surface where
        sizeOf _ = #{size Surface}
        alignment = sizeOf

        peek ptr = Surface <$> (#{peek Surface, hDC} ptr)
                           <*> (#{peek Surface, hBitmap} ptr)

        poke ptr s = do
                #{poke Surface, hDC} ptr $ surfaceDC s
                #{poke Surface, hBitmap} ptr $ surfaceBitmap s

type SurfacePtr = Ptr Surface

surfaceSize :: Surface -> IO (Int, Int)
surfaceSize surface = do
        (_, w, h, _, _, _, _) <- Win32.getBitmapInfo $ surfaceBitmap surface
        return (fromIntegral w, fromIntegral h)

createSurface :: CInt -> CInt -> IO SurfacePtr
createSurface width height = withDC Nothing $ \hTempDC -> do
    hDC <- Win32.createCompatibleDC $ Just hTempDC
    hBitmap <- Win32.createCompatibleBitmap hTempDC (fromIntegral width) (fromIntegral height)
    _ <- Win32.selectBitmap hDC hBitmap
    ptr <- new $ Surface hDC hBitmap
    surfacePaintRect ptr nullPtr (Win32.rgb 0 0 0)
    return ptr

destroySurface :: SurfacePtr -> IO ()
destroySurface surfacePtr
    | surfacePtr == nullPtr = return ()
    | otherwise = do
        surface <- peek surfacePtr
        sequenceA_ $ Win32.deleteBitmap <$> mfilter (/= nullPtr) (Just $ surfaceBitmap surface)
        sequenceA_ $ Win32.deleteDC <$> mfilter (/= nullPtr) (Just $ surfaceDC surface)
        free surfacePtr

surfaceFromBitmapFile :: CString -> IO SurfacePtr
surfaceFromBitmapFile fileName = withDC Nothing $ \hTempDC -> do
    fileName' <- peekCString fileName
    hBitmap <- Win32.loadImage nullPtr fileName' Win32.iMAGE_BITMAP 0 0 (Win32.lR_CREATEDIBSECTION .|. Win32.lR_LOADFROMFILE)
    hDC <- Win32.createCompatibleDC $ Just hTempDC
    _ <- Win32.selectBitmap hDC hBitmap
    new $ Surface hDC hBitmap

surfaceFromResource :: Win32.HINSTANCE -> Win32.LPCTSTR -> IO SurfacePtr
surfaceFromResource hInst resourceName = withDC Nothing $ \hTempDC -> do
    hBitmap <- Win32.failIfNull "LoadImage" $ Win32.c_LoadImage hInst resourceName Win32.iMAGE_BITMAP 0 0 0
    hDC <- Win32.createCompatibleDC $ Just hTempDC
    _ <- Win32.selectBitmap hDC hBitmap
    new $ Surface hDC hBitmap

surfacePaintRect :: SurfacePtr -> Ptr Win32.RECT -> Win32.COLORREF -> IO ()
surfacePaintRect surfacePtr rectPtr color = do
        surface <- peek surfacePtr
        let hDC = surfaceDC surface
        bracket (Win32.createSolidBrush color) Win32.deleteBrush $ \hBrush ->
            bracket (Win32.selectBrush hDC hBrush) (Win32.selectBrush hDC) $ \_ -> do
                if rectPtr == nullPtr
                    then do
                        (w, h) <- surfaceSize surface
                        patBlt hDC 0 0 (fromIntegral w) (fromIntegral h) Win32.pATCOPY
                    else do
                        (l, t, r, b) <- Win32.peekRECT rectPtr
                        patBlt hDC (fromIntegral l) (fromIntegral t) (fromIntegral $ r - l) (fromIntegral $ b - t) Win32.pATCOPY

surfaceDrawText :: SurfacePtr -> CInt -> CInt -> CInt -> Win32.COLORREF -> CString -> IO CInt
surfaceDrawText surfacePtr x y size color text = do
        surface <- peek surfacePtr
        let hDC = surfaceDC surface
        hFont <- Win32.createFont
            (fromIntegral size) 0 0 0
            600 False False False
            Win32.sHIFTJIS_CHARSET
            Win32.oUT_DEFAULT_PRECIS
            Win32.cLIP_DEFAULT_PRECIS
            Win32.dEFAULT_QUALITY
            (Win32.dEFAULT_PITCH .|. Win32.fF_DONTCARE)
            "ＭＳ ゴシック"
        flip finally (Win32.deleteFont hFont) $
            bracket (Win32.setBkMode hDC Win32.tRANSPARENT) (Win32.setBkMode hDC) $ \_ ->
                bracket (Win32.selectFont hDC hFont) (Win32.selectFont hDC) $ \_ ->
                    bracket (Win32.setTextColor hDC color) (Win32.setTextColor hDC) $ \_ -> do
                        textStr <- peekCString text
                        Win32.textOut hDC (fromIntegral x) (fromIntegral y) textStr
                        return 1

surfaceBlit :: SurfacePtr -> CInt -> CInt -> CInt -> CInt -> SurfacePtr -> CInt -> CInt -> Win32.DWORD -> IO ()
surfaceBlit destPtr dx dy width height srcPtr sx sy mode = do
        dest <- peek destPtr
        src <- peek srcPtr
        Win32.bitBlt
            (surfaceDC dest)
            (fromIntegral dx)
            (fromIntegral dy)
            (fromIntegral width)
            (fromIntegral height)
            (surfaceDC src)
            (fromIntegral sx)
            (fromIntegral sy)
            mode

withDC :: Maybe Win32.HWND -> (Win32.HDC -> IO a) -> IO a
withDC hWnd = bracket (Win32.getDC hWnd) (Win32.releaseDC hWnd)

foreign import ccall "windows.h PatBlt" c_patBlt :: Win32.HDC -> Win32.INT -> Win32.INT -> Win32.INT -> Win32.INT -> Win32.RasterOp3 -> IO Bool
patBlt :: Win32.HDC -> Int -> Int -> Int -> Int -> Win32.RasterOp3 -> IO ()
patBlt hDC x y w h rop = Win32.failIfFalse_ "PatBlt" $ c_patBlt
        hDC
        (fromIntegral x)
        (fromIntegral y)
        (fromIntegral w)
        (fromIntegral h)
        rop

foreign export ccall createSurface :: CInt -> CInt -> IO SurfacePtr
foreign export ccall destroySurface :: SurfacePtr -> IO ()
foreign export ccall surfaceFromBitmapFile :: CString -> IO SurfacePtr
foreign export ccall surfaceFromResource :: Win32.HINSTANCE -> Win32.LPCTSTR -> IO SurfacePtr
foreign export ccall surfacePaintRect :: SurfacePtr -> Ptr Win32.RECT -> Win32.COLORREF -> IO ()
foreign export ccall surfaceDrawText :: SurfacePtr -> CInt -> CInt -> CInt -> Win32.COLORREF -> CString -> IO CInt
foreign export ccall surfaceBlit :: SurfacePtr -> CInt -> CInt -> CInt -> CInt -> SurfacePtr -> CInt -> CInt -> Win32.DWORD -> IO ()
