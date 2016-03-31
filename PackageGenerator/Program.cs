using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;

namespace PackageGenerator
{
    public static class Program
    {

        static void Main( string[] args )
        {
            var outputDirPath = System.IO.Path.Combine( Application.StartupPath, "Output" );
            if( !Directory.Exists( outputDirPath ) )
            {
                try
                {
                    Directory.CreateDirectory( outputDirPath );
                }
                catch( Exception ex )
                {
                    Console.WriteLine( ex.Message );
                    Console.ReadKey();
                    return;
                }
            }

            foreach( var fn in Directory.GetFiles( Application.StartupPath, "PackageTemplate_*.dll" ) )
            {
                var asm = Assembly.LoadFile( fn );
                var t = TemplateScaner.GetTemplate( asm );
                var shortfn = new FileInfo( fn ).Name;
                shortfn = shortfn.Substring( 0, shortfn.LastIndexOf( '.' ) );
                t.Name = shortfn.Substring("PackageTemplate_".Length);
                var path = System.IO.Path.Combine( outputDirPath, shortfn.Replace( ".", "_" ) );
                if( !Directory.Exists( path ) )
                {
                    try
                    {
                        Directory.CreateDirectory( path );
                    }
                    catch( Exception ex )
                    {
                        Console.WriteLine( ex.Message );
                        Console.ReadKey();
                        return;
                    }
                }

                //var rtv = GenJava.Gen( t, path, shortfn.Substring( "PackageTemplate_".Length ) );
                //if( !string.IsNullOrEmpty(rtv))
                //{
                //    Console.WriteLine( rtv );
                //    Console.ReadKey();
                //    return;
                //}

                //rtv = GenCS.Gen( t, path, shortfn.Substring( "PackageTemplate_".Length ) );
                //if( !string.IsNullOrEmpty( rtv ) )
                //{
                //    Console.WriteLine( rtv );
                //    Console.ReadKey();
                //    return;
                //}

                //rtv = GenCPP.Gen( t, path, shortfn.Substring( "PackageTemplate_".Length ) );
                //if( !string.IsNullOrEmpty( rtv ) )
                //{
                //    Console.WriteLine( rtv );
                //    Console.ReadKey();
                //    return;
                //}

                var rtv = GenProjCS.Gen(t, path);
                if (!string.IsNullOrEmpty(rtv))
                {
                    Console.WriteLine(rtv);
                    Console.ReadKey();
                    return;
                }

            }

            Process.Start( "explorer.exe", outputDirPath );
        }


    }
}
