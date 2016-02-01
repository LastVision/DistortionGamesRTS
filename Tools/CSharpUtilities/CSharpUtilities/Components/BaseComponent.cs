using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace CSharpUtilities.Components
{
    abstract public class BaseComponent
    {
        protected Point myLocation;
        protected Size mySize;
        protected String myPanelName;

        public BaseComponent(Point aLocation, Size aSize, string aText, string aPanelName)
        {
            myLocation = aLocation;
            mySize = aSize;
            myPanelName = aPanelName;

            InitializeComponents(aText);
        }

        abstract protected void InitializeComponents(string aText);

        abstract public void Show();
        abstract public void Hide();
        virtual public void Update()
        {

        }

        abstract public void BindToPanel(Panel aPanel);
        public String GetPanelName()
        {
            return myPanelName;
        }
    }
}
