using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PackageGenerator
{
    public class GenEmpty
    {
        public static string Gen( Template t, string outDir, string pn )
        {

            var sb = new StringBuilder();





            try
            {
                sb.WriteToFile( Path.Combine( outDir, pn + "_packages.empty" ), false );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }

            return "";
        }

    }
}
