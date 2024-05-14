using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace LEDMoodlightning
{
    public class Animations
    {
        private IView view;
        private string name;
        private byte[] animationsettings;
        private byte animindex;
        private bool buttonenabled;
        public Animations(string name)
        {
            this.name = name;
            animationsettings =new byte[16] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;
            view = null;
        }
        public void setButton()
        {
            view.Update();
        }
        public byte Animindex { 
            get { return animindex; } 
            set { animindex = value; } 
        }
        public string Name
        {
            get { return name; }
         
        }
        public byte[] Animationsettings { 
            get { return animationsettings;}
            set { animationsettings = value; }
        }

        public void AttachView(IView v)
        {
            view = v;
            v.Update();
        }



        public void DetachView(IView v)
        {
            view=null;
        }

        public void UpdateAllViews()
        {
            view.Update();
        }
    }
}