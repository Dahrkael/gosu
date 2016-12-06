using CppSharp;
using CppSharp.AST;
using CppSharp.Passes;
using System;
using System.IO;

namespace Gosu.NETer
{
    class Gosu : ILibrary
    {
        public void Setup(Driver driver)
        {
            var parserOptions = driver.ParserOptions;
            var options = driver.Options;

            options.GeneratorKind = CppSharp.Generators.GeneratorKind.CSharp;

            var gosuPath = GetGosuDirectory();
            var includePath = Path.Combine(gosuPath, "");
            var libraryPath = Path.Combine(gosuPath, "");
            options.LibraryName = "Gosu";
            options.SharedLibraryName = "Gosu.dll";
            //options.Headers.Add(@"Gosu\TR1.hpp");

            options.Headers.Add(@"Gosu\Audio.hpp");
            options.Headers.Add(@"Gosu\Graphics.hpp");
            options.Headers.Add(@"Gosu\GraphicsBase.hpp");
            options.Headers.Add(@"Gosu\Image.hpp");
            options.Headers.Add(@"Gosu\Bitmap.hpp");
            options.Headers.Add(@"Gosu\Font.hpp");
            options.Headers.Add(@"Gosu\Math.hpp");
            options.Headers.Add(@"Gosu\Text.hpp");
            options.Headers.Add(@"Gosu\TextInput.hpp");
            options.Headers.Add(@"Gosu\Window.hpp");
            options.Headers.Add(@"Gosu\Color.hpp");
            options.Headers.Add(@"Gosu\Buttons.hpp");
            options.Headers.Add(@"Gosu\Input.hpp");
            options.Headers.Add(@"Gosu\Timing.hpp");

            parserOptions.AddIncludeDirs(includePath);
            parserOptions.AddLibraryDirs(libraryPath);
            options.OutputDir = "Gosu.NET";
            options.CommentPrefix = "//";
            options.GenerateDefaultValuesForArguments = true;
            options.OutputNamespace = "GosuNET";
            options.IncludePrefix = "Gosu/";

            if (options.IsCLIGenerator)
            {
                if (Directory.Exists(@"..\..\..\..\Gosu.NET-CLI"))
                {
                    options.OutputDir = @"..\..\..\..\Gosu.NET-CLI";
                }
            }
            if (options.IsCSharpGenerator)
            {
                if (Directory.Exists(@"..\..\..\..\Gosu.NET"))
                {
                    options.OutputDir = @"..\..\..\..\Gosu.NET";
                }
            }
        }

        public void SetupPasses(Driver driver)
        {
            driver.Context.TranslationUnitPasses.RenameDeclsUpperCase(RenameTargets.Any);
            driver.Context.TranslationUnitPasses.AddPass(new FunctionToInstanceMethodPass());
            /*
            driver.Context.TranslationUnitPasses.RemovePrefix("SDL_");
            driver.Context.TranslationUnitPasses.RemovePrefix("SCANCODE_");
            driver.Context.TranslationUnitPasses.RemovePrefix("SDLK_");
            driver.Context.TranslationUnitPasses.RemovePrefix("KMOD_");
            driver.Context.TranslationUnitPasses.RemovePrefix("LOG_CATEGORY_");
            */
        }

        public void Preprocess(Driver driver, ASTContext ctx)
        {
            ctx.IgnoreHeadersWithName("AutoLink.hpp");
            ctx.IgnoreHeadersWithName("Directories.hpp");
            ctx.IgnoreHeadersWithName("Fwd.hpp");
            ctx.IgnoreHeadersWithName("Gosu.hpp");
            //ctx.IgnoreHeadersWithName("GraphicsBase.hpp");
            ctx.IgnoreHeadersWithName("ImageData.hpp");
            ctx.IgnoreHeadersWithName("IO.hpp");
            //ctx.IgnoreHeadersWithName("Math.hpp");
            ctx.IgnoreHeadersWithName("Platform.hpp");
            ctx.IgnoreHeadersWithName("Sockets.hpp");
            ctx.IgnoreHeadersWithName("TR1.hpp");
            ctx.IgnoreHeadersWithName("Utility.hpp");
            ctx.IgnoreHeadersWithName("Version.hpp");
            /*
            ctx.IgnoreEnumWithMatchingItem("SDL_FALSE");
            ctx.IgnoreEnumWithMatchingItem("DUMMY_ENUM_VALUE");

            ctx.SetNameOfEnumWithMatchingItem("SDL_SCANCODE_UNKNOWN", "ScanCode");
            ctx.SetNameOfEnumWithMatchingItem("SDLK_UNKNOWN", "Key");
            ctx.SetNameOfEnumWithMatchingItem("KMOD_NONE", "KeyModifier");
            ctx.SetNameOfEnumWithMatchingItem("SDL_LOG_CATEGORY_CUSTOM", "LogCategory");

            ctx.GenerateEnumFromMacros("InitFlags", "SDL_INIT_(.*)").SetFlags();
            ctx.GenerateEnumFromMacros("Endianness", "SDL_(.*)_ENDIAN");
            ctx.GenerateEnumFromMacros("InputState", "SDL_RELEASED", "SDL_PRESSED");
            ctx.GenerateEnumFromMacros("AlphaState", "SDL_ALPHA_(.*)");
            ctx.GenerateEnumFromMacros("HatState", "SDL_HAT_(.*)")

            ctx.IgnoreEnumWithMatchingItem("SDL_ENOMEM");
            ctx.IgnoreFunctionWithName("SDL_Error");
            */
        }

        public void Postprocess(Driver driver, ASTContext ctx)
        {
            /*
            ctx.SetNameOfEnumWithName("PIXELTYPE", "PixelType");
            ctx.SetNameOfEnumWithName("BITMAPORDER", "BitmapOrder");
            ctx.SetNameOfEnumWithName("PACKEDORDER", "PackedOrder");
            ctx.SetNameOfEnumWithName("ARRAYORDER", "ArrayOrder");
            ctx.SetNameOfEnumWithName("PACKEDLAYOUT", "PackedLayout");
            ctx.SetNameOfEnumWithName("PIXELFORMAT", "PixelFormats");
            ctx.SetNameOfEnumWithName("assert_state", "AssertState");
            ctx.SetClassBindName("assert_data", "AssertData");
            ctx.SetNameOfEnumWithName("eventaction", "EventAction");
            ctx.SetNameOfEnumWithName("LOG_CATEGORY", "LogCategory");
            */
        }

        public static string GetGosuDirectory()
        {
            var directory = @"H:\Proyectos\gosu";
            //var directory = string.Format(@"{0}\..\..\..\gosu", Directory.GetCurrentDirectory());

            if (Directory.Exists(directory))
            {
                return directory;
            }
            else
            {
                throw new Exception("gosu directory for project was not found");
            }
        }

        static class Program
        {
            public static void Main(string[] args)
            {
                ConsoleDriver.Run(new Gosu());
            }
        }
    }
}