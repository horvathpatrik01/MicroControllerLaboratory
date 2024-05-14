using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace LEDMoodlightning
{
    public partial class MoodLED : Form
    {

        private static string[] prevPorts = new string[255];
        private int animcount;
        private string errormessage;
        public MoodLED()
        {
            InitializeComponent();
            Timer tmr = new Timer();
            tmr.Interval = 100;   // milliseconds
            tmr.Tick += UpdateData;
            tmr.Start();
            animcount = 0;
        }

        public int Animcount { 
            get { return animcount; }
            set { animcount = value; }
        }
        public string Errormessage { set { errormessage = value; } }
        private void UpdateData(object sender, EventArgs e)
        {
            //Portok ellenörzése, frissítése
            string[] ports = SerialPort.GetPortNames();
            if (!prevPorts.SequenceEqual(ports))
            {
                prevPorts = ports;
                PortBox.Items.Clear();
                PortBox.Text = "";
                if (ports.Length > 0)
                {
                    B_OpenPort.Enabled = true;
                    PortBox.Items.AddRange(ports);
                    PortBox.SelectedIndex = 0;
                }
                else if (ports.Length == 0)
                {
                    B_OpenPort.Enabled = false;
                }
            }
            //tab eltüntetése ha üres
            if(tabControl1.TabCount==0)
                tabControl1.Visible = false;
        }

        private void MoodLED_Load(object sender, EventArgs e)
        {

        }

        private void fontDialog1_Apply(object sender, EventArgs e)
        {
        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void selectUARTPortToolStripMenuItem_Click(object sender, EventArgs e)
        {
            groupBox1.Visible = true;
            tabControl1.Visible = false;
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (animcount == 8)
                return;
            App.Instance.NewAnimation();
        }

        private void B_OpenPort_MouseClick(object sender, MouseEventArgs e)
        {
            //Port nyitása
            if (PortBox.Items.Count > 0)
            {
                B_OpenPort.Enabled = false;
                B_ClosePort.Enabled = true;
                try
                {
                    serialPort1.PortName = PortBox.Text;
                    serialPort1.BaudRate = 115200;
                    serialPort1.DataBits = 8;
                    serialPort1.StopBits = StopBits.One;
                    serialPort1.Open();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Nem sikerült megnyitni a portot!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        //soros port bontása
        private void B_ClosePort_MouseClick(object sender, MouseEventArgs e)
        {
            serialPort1.Close();
            B_OpenPort.Enabled = true;
            B_ClosePort.Enabled = false;
        }

        private void MoodLED_FormClosed(object sender, FormClosedEventArgs e)
        {
            serialPort1.Close();
        }
        public TabControl TabControl
        {
            get { return tabControl1; }
        }
        public SerialPort SerialPort1 { 
            get { return serialPort1; } 
        }

        public Button OpenPort
        {
            get { return B_OpenPort; }
        }
        private void settingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            groupBox1.Visible = false;
            if(tabControl1.TabCount > 0)
            tabControl1.Visible = true;
            App.Instance.SetLoadButtonEnabled();    
        }

        //aktív animáció törlése
        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            App.Instance.CloseActiveAnimation();
            
        }

        //tabváltás esetén load engedélyezése és az aktív animation átállítása
        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            App.Instance.UpdateActiveAnimation();
            App.Instance.SetLoadButtonEnabled();
        }

        private void settingsToolStripMenuItem1_Click(object sender, EventArgs e)
        {

        }
    }
}
