using System;
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
    public partial class AnimView : UserControl, IView
    {
        private Animations animation;
        private byte[] animationsettingvalues;

        public AnimView()
        {
            InitializeComponent();
        }

        public Button LaunchButton
        {
            get { return B_LoadAnim; }
        }

        //load gomb engedélyezése
        public void setButtonenabled()
        {
            LaunchButton.Enabled = App.ButtonEnabled;
        }

        public void Update()
        {
            setButtonenabled();
            Invalidate();
        }

        public void AttachToAnim(Animations anim)
        {
            anim.AttachView(this);
            animation=anim;
        }

        //ha a négyszögjel van bepipálva akkor a hozzá tartozó ablak jelenik meg.
        private void CB_RedSignalTypeSquare_CheckedChanged(object sender, EventArgs e)
        {
            if (CB_RedSignalTypeSquare.Checked)
            {
                GB_RedSquarePeriods.Visible = true;
                GB_RedRampDutyCycle.Visible = false;
                CB_RedSignalTypeRamp.Enabled = false;
            }
            else
            {
                GB_RedSquarePeriods.Visible = false;
                CB_RedSignalTypeRamp.Enabled = true;
            }
        }

        //ha a háromszögjel van bepipálva akkor a hozzá tartozó ablak jelenik meg.
        private void CB_RedSignalTypeRamp_CheckedChanged(object sender, EventArgs e)
        {
            if (CB_RedSignalTypeRamp.Checked)
            {
                GB_RedRampDutyCycle.Visible = true;
                GB_RedSquarePeriods.Visible = false;
                CB_RedSignalTypeSquare.Enabled = false;
            }
            else
            {
                GB_RedRampDutyCycle.Visible = false;
                CB_RedSignalTypeSquare.Enabled = true;
            }
        }

        private void hSB_RedSquarePeriods_ValueChanged(object sender, EventArgs e)
        {
            label8.Text=this.hSB_RedSquarePeriods.Value.ToString();
        }

        private void hSB_GreenSquarePeriods_ValueChanged(object sender, EventArgs e)
        {
            label9.Text=this.hSB_GreenSquarePeriods.Value.ToString();
        }

        private void hSB_BlueSquarePeriods_ValueChanged(object sender, EventArgs e)
        {
            label10.Text=this.hSB_BlueSquarePeriods.Value.ToString();
        }

        private void CB_GreenSignalTypeSquare_CheckedChanged(object sender, EventArgs e)
        {
            if (CB_GreenSignalTypeSquare.Checked)
            {
                GB_GreenSquarePeriods.Visible = true;
                GB_GreenRampDutyCycle.Visible = false;
                CB_GreenSignalTypeRamp.Enabled = false;
            }
            else
            {
                GB_GreenSquarePeriods.Visible = false;
                CB_GreenSignalTypeRamp.Enabled = true;
            }

        }

        private void CB_GreenSignalTypeRamp_CheckedChanged(object sender, EventArgs e)
        {
            if (CB_GreenSignalTypeRamp.Checked)
            {
                GB_GreenRampDutyCycle.Visible = true;
                GB_GreenSquarePeriods.Visible = false;
                CB_GreenSignalTypeSquare.Enabled = false;
            }
            else
            {
                GB_GreenRampDutyCycle.Visible = false;
                CB_GreenSignalTypeSquare.Enabled = true;
            }
        }

        private void CB_BlueSignalTypeSquare_CheckedChanged(object sender, EventArgs e)
        {
            if (CB_BlueSignalTypeSquare.Checked)
            {
                GB_BlueSquarePeriods.Visible = true;
                GB_BlueRampDutyCycle.Visible = false;
                CB_BlueSignalTypeRamp.Enabled = false;
            }
            else
            {
                GB_BlueSquarePeriods.Visible = false;
                CB_BlueSignalTypeRamp.Enabled = true;
            }
        }
        private void CB_BlueSignalTypeRamp_CheckedChanged(object sender, EventArgs e)
        {
            if (CB_BlueSignalTypeRamp.Checked)
            {
                GB_BlueRampDutyCycle.Visible = true;
                GB_BlueSquarePeriods.Visible = false;
                CB_BlueSignalTypeSquare.Enabled = false;
            }
            else
            {
                GB_BlueRampDutyCycle.Visible = false;
                CB_BlueSignalTypeSquare.Enabled = true;
            }
        }

       

        private void CB_RedRampDC100_CheckedChanged(object sender, EventArgs e)
        {
            if(CB_RedRampDC100.Checked)
                CB_RedRampDC50.Enabled = false;
            else
                CB_RedRampDC50.Enabled = true;
        }

        private void CB_RedRampDC50_CheckedChanged(object sender, EventArgs e)
        {
            if (CB_RedRampDC50.Checked)
                CB_RedRampDC100.Enabled = false;
            else
                CB_RedRampDC100.Enabled = true;
        }

        private void CB_GreenRampDC100_CheckedChanged(object sender, EventArgs e)
        {
            if (CB_GreenRampDC100.Checked)
                CB_GreenRampDC50.Enabled = false;
            else
                CB_GreenRampDC50.Enabled = true;
        }

        private void CB_GreenRampDC50_CheckedChanged(object sender, EventArgs e)
        {
            if (CB_GreenRampDC50.Checked)
                CB_GreenRampDC100.Enabled = false;
            else
                CB_GreenRampDC100.Enabled = true;
        }

        private void CB_BlueRampDC100_CheckedChanged(object sender, EventArgs e)
        {

            if (CB_BlueRampDC100.Checked)
                CB_BlueRampDC50.Enabled = false;
            else
                CB_BlueRampDC50.Enabled = true;
        }

        private void CB_BlueRampDC50_CheckedChanged(object sender, EventArgs e)
        {
            if (CB_BlueRampDC50.Checked)
                CB_BlueRampDC100.Enabled = false;
            else
                CB_BlueRampDC100.Enabled = true;
        }

        //Adatok beolvasása, hibakezelés, adatok átadása az animation osztálynak valamint a küldő függvény hívása és a nyugtázó üzenet fogadása
        private void B_LoadAnim_MouseClick(object sender, MouseEventArgs e)
        {
            B_LoadAnim.Enabled = false;
            animationsettingvalues = new byte[16] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0 };
            byte canwestart = 1;
            uint value;
            try
            {
                if (!uint.TryParse(TB_RedPWMStart.Text, out value))
                {
                    canwestart = 0;
                    throw new FormatException();
                }
                if(value<0&& value > 255) {
                    canwestart = 0;
                    throw new ArgumentOutOfRangeException();
                }
                else
                    animationsettingvalues[1] = (byte)value;
                if (!uint.TryParse(TB_RedPWMStop.Text, out value))
                {
                    canwestart = 0;
                    throw new FormatException();
                }

                if (value < 0 && value > 255)
                {
                    canwestart = 0;
                    throw new ArgumentOutOfRangeException();
                }
                else
                    animationsettingvalues[2] = (byte)value;
                if (!uint.TryParse(TB_GreenPWMStart.Text, out value))
                {
                    canwestart = 0;
                    throw new FormatException();
                }

                if (value < 0 && value > 255)
                {
                    canwestart = 0;
                    throw new ArgumentOutOfRangeException();
                }
                else
                    animationsettingvalues[4] = (byte)value;
                if (!uint.TryParse(TB_GreenPWMStop.Text, out value))
                {
                    canwestart = 0;
                    throw new FormatException();
                }

                if (value < 0 && value > 255)
                {
                    canwestart = 0;
                    throw new ArgumentOutOfRangeException();
                }
                else
                    animationsettingvalues[5] = (byte)value;
                if (!uint.TryParse(TB_BluePWMStart.Text, out value))
                {
                    canwestart = 0;
                    throw new FormatException();
                }

                if (value < 0 && value > 255)
                {
                    canwestart = 0;
                    throw new ArgumentOutOfRangeException();
                }
                else
                    animationsettingvalues[7] = (byte)value;
                if (!uint.TryParse(TB_BluePWMStop.Text, out value))
                {
                    canwestart = 0;
                    throw new FormatException();
                }
                if (value < 0 && value > 255)
                {
                    canwestart = 0;
                    throw new ArgumentOutOfRangeException();
                }
                else
                    animationsettingvalues[8] = (byte)value;
                if (!uint.TryParse(TB_sec.Text, out value))
                {
                    canwestart = 0;
                    throw new FormatException();
                }

                if (value <0 && value > 30)
                {
                    canwestart = 0;
                    throw new ArgumentOutOfRangeException();
                }
                else
                    animationsettingvalues[10] = (byte)value;

                if (!uint.TryParse(TB_msec.Text, out value))
                {
                    canwestart = 0;
                    throw new FormatException();
                }
                if (value < 0 && value > 30000)
                {
                    canwestart = 0;
                    throw new ArgumentOutOfRangeException();
                }
                else
                {
                    uint tmp;
                    if(value>999)
                    {
                        tmp = animationsettingvalues[10]+(value / 1000);
                        if (tmp > 30)
                        {
                            canwestart = 0;
                            throw new ArgumentOutOfRangeException();
                        }
                        else
                        {
                            value = value % 1000;
                            animationsettingvalues[10] = (byte)tmp;
                            TB_sec.Text = tmp.ToString();
                            TB_msec.Text = value.ToString();
                        }
                    }
                    if(animationsettingvalues[10] == 0&&value<100)
                    {
                        canwestart = 0;
                        throw new ArgumentOutOfRangeException();
                    }
                    animationsettingvalues[11] = (byte)(value/10);
                }
                if (!CB_RedSignalTypeSquare.Checked && (!CB_RedSignalTypeRamp.Checked))
                {
                    canwestart = 0;
                    throw new MissingFieldException();
                }
                else
                {
                    if (CB_RedSignalTypeSquare.Checked)
                    {
                        animationsettingvalues[12] = (byte)hSB_RedSquarePeriods.Value;
                    }
                    else
                        animationsettingvalues[12] = 0;
                }
                if (!CB_GreenSignalTypeSquare.Checked && (!CB_GreenSignalTypeRamp.Checked))
                {
                    canwestart = 0;
                    throw new MissingFieldException();
                }
                else
                {
                    if (CB_GreenSignalTypeSquare.Checked)
                    {
                        animationsettingvalues[13] = (byte)hSB_GreenSquarePeriods.Value;
                    }
                    else
                        animationsettingvalues[13] = 0;
                }
                if (!CB_BlueSignalTypeSquare.Checked && (!CB_BlueSignalTypeRamp.Checked))
                {
                    canwestart = 0;
                    throw new MissingFieldException();
                }
                else
                {
                    if (CB_BlueSignalTypeSquare.Checked)
                    {
                        animationsettingvalues[14] = (byte)hSB_BlueSquarePeriods.Value;
                    }
                    else
                        animationsettingvalues[14] = 0;
                }
                if(CB_RedSignalTypeRamp.Checked&&(!CB_RedRampDC50.Checked && (!CB_RedRampDC100.Checked)))
                {
                    canwestart = 0;
                    throw new MissingFieldException();
                }
                else
                {
                    if (CB_RedRampDC50.Checked)
                        animationsettingvalues[3] = 1;
                    else
                        animationsettingvalues[3] = 0;
                }
                if (CB_GreenSignalTypeRamp.Checked&&(!CB_GreenRampDC50.Checked && (!CB_GreenRampDC100.Checked)))
                {
                    canwestart = 0;
                    throw new MissingFieldException();
                }
                else
                {
                    if (CB_GreenRampDC50.Checked)
                        animationsettingvalues[6] = 1;
                    else
                        animationsettingvalues[6] = 0;
                }
                if (CB_BlueSignalTypeRamp.Checked&&(!CB_BlueRampDC50.Checked && (!CB_BlueRampDC100.Checked)))
                {
                    canwestart = 0;
                    throw new MissingFieldException();
                }
                else
                {
                    if (CB_BlueRampDC50.Checked)
                        animationsettingvalues[9] = 1;
                    else
                        animationsettingvalues[9] = 0;
                }

                animationsettingvalues[0] = canwestart;


            }
            catch (FormatException except){ Console.WriteLine("Hiba :" + except.ToString()); }
            catch (ArgumentOutOfRangeException except) { Console.WriteLine("Hiba :" + except.ToString()); }
            catch (MissingFieldException except) { Console.WriteLine("Hiba :" + except.ToString()); }
            catch (Exception except){ Console.WriteLine("Hiba :" + except.ToString()); }
            finally
            {
                animation.Animationsettings = animationsettingvalues;
                /*for(int i = 0; i < animationsettingvalues.Length; i++)
                {
                    Console.WriteLine(animationsettingvalues[i]);
                }*/
                if (canwestart == 1)
                {
                    App.Instance.SendAnimationSettings(false);
                    App.Instance.RecieveAcknowledge();
                    if (App.Acknowledge != null)
                    {
                        if (String.Compare(App.Acknowledge, "OK") == 0)
                        {
                            Console.WriteLine("The selected animation has been added.");
                            MessageBox.Show(App.Acknowledge, "The selected animation has been added.", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        }
                        else if (String.Compare(App.Acknowledge, "ERROR") == 0)
                        {
                            Console.WriteLine("There are some problems with the bytes has been sent.");
                            MessageBox.Show(App.Acknowledge, "There are some problems with the bytes has been sent.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        }
                        App.Acknowledge = null;
                    }
                    else
                    {
                        Console.WriteLine("Nothing happened");
                    }
                }
                B_LoadAnim.Enabled = true;
            }



        }
        //random animáció értékek betöltése
        private void B_RandomAnim_MouseClick(object sender, MouseEventArgs e)
        {
            Random rnd = new Random();
            TB_RedPWMStart.Text = rnd.Next(0, 256).ToString();
            TB_RedPWMStop.Text = rnd.Next(0, 256).ToString();
            TB_GreenPWMStart.Text = rnd.Next(0, 256).ToString();
            TB_GreenPWMStop.Text = rnd.Next(0, 256).ToString();
            TB_BluePWMStart.Text = rnd.Next(0, 256).ToString();
            TB_BluePWMStop.Text = rnd.Next(0, 256).ToString();
            CB_RedSignalTypeSquare.Checked = rnd.Next(0,2).Equals(1);
            CB_GreenSignalTypeSquare.Checked = rnd.Next(0, 2).Equals(1);
            CB_BlueSignalTypeSquare.Checked = rnd.Next(0, 2).Equals(1);
            if (!CB_RedSignalTypeSquare.Checked)
                CB_RedSignalTypeRamp.Checked = true;
            else
                CB_RedSignalTypeRamp.Checked = false;
            if (!CB_GreenSignalTypeSquare.Checked)
                CB_GreenSignalTypeRamp.Checked = true;
            else
                CB_GreenSignalTypeRamp.Checked = false;
            if (!CB_BlueSignalTypeSquare.Checked)
                CB_BlueSignalTypeRamp.Checked = true;
            else
                CB_BlueSignalTypeRamp.Checked = false;
            if (CB_RedSignalTypeRamp.Checked)
            {
                CB_RedRampDC100.Checked = rnd.Next(0, 2).Equals(1);
                if(CB_RedRampDC100.Checked)
                    CB_RedRampDC50.Checked = false;
                else
                    CB_RedRampDC50.Checked = true;
            }
            if (CB_GreenSignalTypeRamp.Checked)
            {
                CB_GreenRampDC50.Checked = rnd.Next(0, 2).Equals(1);
                if (CB_GreenRampDC50.Checked)
                    CB_GreenRampDC100.Checked = false;
                else
                    CB_GreenRampDC100.Checked = true;
            }
            if (CB_BlueSignalTypeRamp.Checked)
            {
                CB_BlueRampDC50.Checked = rnd.Next(0,2).Equals(1);
                if (CB_BlueRampDC50.Checked)
                    CB_BlueRampDC100.Checked = false;
                else
                    CB_BlueRampDC100.Checked = true;
            }
            hSB_RedSquarePeriods.Value = rnd.Next(1,5);
            hSB_GreenSquarePeriods.Value = rnd.Next(1, 5);
            hSB_BlueSquarePeriods.Value = rnd.Next(1, 5);
            TB_sec.Text = rnd.Next(0,5).ToString();
            TB_msec.Text = rnd.Next(500,1001).ToString();
        }
        //beállítás adatok törlése
        private void B_ClearAnim_MouseClick(object sender, MouseEventArgs e)
        {
            TB_RedPWMStart.Text = "";
            TB_RedPWMStop.Text = "";
            TB_GreenPWMStart.Text = "";
            TB_GreenPWMStop.Text = "";
            TB_BluePWMStart.Text = "";
            TB_BluePWMStop.Text = "";
            CB_RedSignalTypeSquare.Checked = false;
            CB_GreenSignalTypeSquare.Checked = false;
            CB_BlueSignalTypeSquare.Checked = false;
            CB_RedSignalTypeRamp.Checked = false;
            CB_GreenSignalTypeRamp.Checked = false;
            CB_BlueSignalTypeRamp.Checked = false;
            CB_RedRampDC100.Checked = false;
            CB_RedRampDC50.Checked = false;
            CB_GreenRampDC50.Checked = false;
            CB_GreenRampDC100.Checked = false;
            CB_BlueRampDC50.Checked = false;
            CB_BlueRampDC100.Checked = false;
            hSB_RedSquarePeriods.Value = 1;
            hSB_GreenSquarePeriods.Value = 1;
            hSB_BlueSquarePeriods.Value = 1;
            TB_sec.Text = "";
            TB_msec.Text = "";
        }

        private void label11_Click(object sender, EventArgs e)
        {

        }
    }
}
