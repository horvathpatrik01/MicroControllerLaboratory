﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace LEDMoodlightning
{
    public partial class NewAnimForm : Form
    {
        public NewAnimForm()
        {
            InitializeComponent();
        }

        public string FontName
        {
            get { return TB_NewAnim.Text; }
        }

        private void B_Closecreation_MouseClick(object sender, MouseEventArgs e)
        {
            this.Close();
        }
    }

}
