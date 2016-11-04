import Distribution.PackageDescription (PackageDescription(extraSrcFiles))
import Distribution.Simple
import Distribution.Simple.LocalBuildInfo (LocalBuildInfo)
import Distribution.Simple.Setup (BuildFlags(..), fromFlagOrDefault)
import Distribution.Simple.Utils (createDirectoryIfMissingVerbose)
import qualified Distribution.Verbosity as V
import System.Process (system)

main :: IO ()
main = defaultMainWithHooks simpleUserHooks {buildHook = resourceHook}

resourceHook :: PackageDescription -> LocalBuildInfo -> UserHooks -> BuildFlags -> IO ()
resourceHook packageDesc localBuildInfo userHooks buildFlags = do
        createDirectoryIfMissingVerbose (fromFlagOrDefault V.silent $ buildVerbosity buildFlags) False "dist"
        system "windres src-c/same.rc dist/resource.o"
        (buildHook simpleUserHooks) packageDesc localBuildInfo userHooks buildFlags
