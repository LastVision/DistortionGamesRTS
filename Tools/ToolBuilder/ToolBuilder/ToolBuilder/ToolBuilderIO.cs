using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ToolBuilder.ToolBuilder
{
    enum eIOType
    {
        IN,
        OUT,
        INOUT
    };

    struct ToolBuilderIO
    {
        public eIOType myIOType;
        public String myIOComponentType;
        public String myIOElementName;
        public String[] myIOAttribute;
    }
}
