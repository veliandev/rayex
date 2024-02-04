 defmodule Rayex.BundlexProject do
   use Bundlex.Project

   def project() do
     [
       natives: natives(Bundlex.platform())
     ]
   end

   def natives(_platform) do
     [
       rayex: [
         language: :cpp,
         lib_dirs: ["#{File.cwd!()}/c_src/rayex/lib"],
         includes: ["#{File.cwd!()}/c_src/rayex/include"],
         sources: ["rayex.cpp"],
         interface: [:nif, :cnode],
         libs: ["raylib", "GL", "m", "pthread", "dl", "rt", "X11"],
         preprocessor: Unifex
       ]
     ]
   end
 end
