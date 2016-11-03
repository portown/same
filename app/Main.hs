module Main where

import Control.Monad (void)
import Foreign.C.Types (CInt(..))

main :: IO ()
main = void cMain

foreign import ccall "c_main" cMain :: IO CInt
